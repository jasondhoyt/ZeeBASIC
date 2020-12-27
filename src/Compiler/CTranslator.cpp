// BSD 2-Clause License
//
// Copyright (c) 2020, Jason Hoyt
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE

#include <cassert>
#include <stdexcept>

#include "ZeeBasic/Compiler/CTranslator.hpp"

#include "ZeeBasic/Compiler/AssignmentStatementNode.hpp"
#include "ZeeBasic/Compiler/BinaryExpressionNode.hpp"
#include "ZeeBasic/Compiler/FunctionCallExpressionNode.hpp"
#include "ZeeBasic/Compiler/IdentifierExpressionNode.hpp"
#include "ZeeBasic/Compiler/IntegerLiteralNode.hpp"
#include "ZeeBasic/Compiler/PrintStatementNode.hpp"
#include "ZeeBasic/Compiler/Program.hpp"
#include "ZeeBasic/Compiler/StringLiteralNode.hpp"

namespace ZeeBasic::Compiler
{

	CTranslator::Writer::Writer(std::vector<VariableIndex>& variableIndices)
		:
		m_variableIndices(variableIndices)
	{ }

	void CTranslator::Writer::setFile(FILE* file)
	{
		m_outFile = file;
	}

	void CTranslator::Writer::indent()
	{
		for (int i = 0; i < m_indent; ++i)
		{
			fputs("    ", m_outFile);
		}
	}

	void CTranslator::Writer::pushIndent()
	{
		m_indent++;
	}

	void CTranslator::Writer::popIndent()
	{
		m_indent--;
	}

	CTranslator::Writer& CTranslator::Writer::operator<<(char ch)
	{
		fputc(ch, m_outFile);
		return *this;
	}

	CTranslator::Writer& CTranslator::Writer::operator<<(const char* text)
	{
		fputs(text, m_outFile);
		return *this;
	}

	CTranslator::Writer& CTranslator::Writer::operator<<(int64_t value)
	{
		fprintf(m_outFile, "%lld", value);
		return *this;
	}

	CTranslator::Writer& CTranslator::Writer::operator<<(int index)
	{
		return operator<<(m_variableIndices[index]);
	}

	CTranslator::Writer& CTranslator::Writer::operator<<(const VariableIndex& index)
	{
		if (index.indexType == IndexType::Local)
		{
			return operator<<(*index.symbol);
		}
		else
		{
			fprintf(m_outFile, "t_%d", index.id);
		}

		return *this;
	}

	CTranslator::Writer& CTranslator::Writer::operator<<(const Symbol& symbol)
	{
		fputs("v_", m_outFile);

		auto len = symbol.name.getLength();
		auto lastChar = symbol.name.getText()[len - 1];
		auto appendChar = char{ 0 };

		if (lastChar == '$')
		{
			appendChar = 's';
		}

		if (appendChar == 0)
		{
			// just write out whole name
			fputs(symbol.name.getText(), m_outFile);
		}
		else
		{
			fwrite(symbol.name.getText(), sizeof(char), len - 1, m_outFile);
			fputc('_', m_outFile);
			fputc(appendChar, m_outFile);
		}

		return *this;
	}


	CTranslator::CTranslator(const std::string& path, const Program& program)
		:
		ITranslator(),
		m_program(program),
		m_writer(m_variableIndices)
	{
#ifdef _WIN32
		fopen_s(&m_file, path.c_str(), "w");
#else
		m_file = fopen(path.c_str(), "w");
#endif
		if (!m_file)
		{
			throw std::runtime_error(std::string{ "Failed to open file for writing : " } + path);
		}

		m_writer.setFile(m_file);
	}

	CTranslator::~CTranslator()
	{
		if (m_file)
		{
			fclose(m_file);
		}
	}

	void CTranslator::run()
	{
		fprintf(m_file, "#include <ZeeBasic/Runtime/ZeeRuntime.h>\n");
		fprintf(m_file, "\n");
		fprintf(m_file, "void program(void)\n");
		fprintf(m_file, "{\n");

		m_writer.pushIndent();

		// initialize locals
		const auto& symbols = m_program.symbols.getSymbols();
		for (auto& symbol : symbols)
		{
			m_writer.indent();
			
			switch (symbol->type.base)
			{

			case BaseType_Integer:
				m_writer << "zrt_Int " << *symbol << " = 0;\n";
				break;

			case BaseType_String:
				m_writer << "zrt_String* " << *symbol << " = zrt_str_empty();\n";
				break;

			default:
				assert(false);				

			}
		}

		for (const auto& stm : m_program.statements)
		{
			stm->translate(*this);
		}

		// cleanup locals (in reverse order)
		for (auto it = symbols.rbegin(); it != symbols.rend(); ++it)
		{
			auto& symbol = *it;

			if (symbol->type.base == BaseType_String)
			{
				m_writer.indent();
				m_writer << "zrt_str_del(" << *symbol << ");\n";
			}
		}

		fprintf(m_file, "}\n");
		fprintf(m_file, "\n");
		fprintf(m_file, "int main(int argc, char* argv[])\n");
		fprintf(m_file, "{\n");
		fprintf(m_file, "    zrt_init(argc, argv);\n");
		fprintf(m_file, "    program();\n");
		fprintf(m_file, "    return 0;\n");
		fprintf(m_file, "}\n");

		fflush(m_file);
	}

	void CTranslator::translate(const Nodes::AssignmentStatementNode& node)
	{
		node.getExpression()->translate(*this);
		
		auto index = popIndex();
		
		m_writer.indent();
		if (index.type.base == BaseType_String)
		{
			// TODO (copy string?)
			m_writer << "zrt_str_copy(" << *node.getSymbol() << ", " << index << ");\n";
		}
		else
		{
			m_writer << *node.getSymbol() << " = " << index << ";\n";
		}

		destroyIndex(index);
	}

	void CTranslator::translate(const Nodes::BinaryExpressionNode& node)
	{
		node.getLeft().translate(*this);
		node.getRight().translate(*this);

		auto rhs = popIndex();
		auto lhs = popIndex();

		m_writer.indent();
		if (lhs.type.base == BaseType_Integer)
		{
			auto ix = pushIndex(BaseType_Integer);
			char op = '?';
			switch (node.getOperator())
			{

			case Nodes::BinaryExpressionNode::Operator::Add:
				op = '+';
				break;

			case Nodes::BinaryExpressionNode::Operator::Subtract:
				op = '-';
				break;

			case Nodes::BinaryExpressionNode::Operator::Multiply:
				op = '*';
				break;

			case Nodes::BinaryExpressionNode::Operator::Divide:
				op = '/';
				break;

			default:
				assert(false);

			}

			m_writer << "zrt_Int " << ix << " = " << lhs << " " << op << " " << rhs << ";\n";
		}
		else if (lhs.type.base == BaseType_String)
		{
			assert(node.getOperator() == Nodes::BinaryExpressionNode::Operator::Add);
			auto ix = pushIndex(BaseType_String);
			m_writer << "zrt_String* " << ix << " = zrt_str_concat(" << lhs << ", " << rhs << ");\n";
		}

		destroyIndex(rhs);
		destroyIndex(lhs);	
	}

	void CTranslator::translate(const Nodes::FunctionCallExpressionNode& node)
	{
		auto& args = node.getArguments();
		for (auto& arg : args)
		{
			arg->translate(*this);
		}

		// TODO : mapping
		if (node.getName() == "STR$")
		{
			auto index = popIndex();
			assert(index.type.base == BaseType_Integer);

			auto ix = pushIndex(BaseType_String);
			m_writer.indent();
			m_writer << "zrt_String* " << ix << " = zrt_str_new_from_int(" << index << ");\n";

			destroyIndex(index);
		}

	}

	void CTranslator::translate(const Nodes::IdentifierExpressionNode& node)
	{
		pushIndex(node.getSymbol());
	}

	void CTranslator::translate(const Nodes::IntegerLiteralNode& node)
	{
		auto ix = pushIndex(BaseType_Integer);
		m_writer.indent();
		m_writer << "zrt_Int " << ix << " = " << node.getValue() << ";\n";
	}

	void CTranslator::translate(const Nodes::PrintStatementNode& node)
	{
		const auto expr = node.getExpression();
		if (expr)
		{
			expr->translate(*this);
			auto index = popIndex();

			m_writer.indent();
			switch (index.type.base)
			{

			case BaseType_Integer:
				m_writer << "zrt_println_int(" << index << ");\n";
				break;

			case BaseType_String:
				m_writer << "zrt_println_str(" << index << ");\n";
				break;

			default:
				assert(false);
				break;

			}

			destroyIndex(index);
		}
		else
		{
			m_writer.indent();
			m_writer << "zrt_println();\n";
		}
	}

	void CTranslator::translate(const Nodes::StringLiteralNode& node)
	{
		auto ix = pushIndex(BaseType_String);
		m_writer.indent();
		m_writer << "zrt_String* " << ix << " = zrt_str_new(\"" << node.getValue().getText() << "\");\n";
	}

	int CTranslator::pushIndex(const Symbol& symbol)
	{
		m_variableIndices.emplace_back(symbol);
		return int(m_variableIndices.size() - 1);
	}

	int CTranslator::pushIndex(const Type& type)
	{
		m_variableIndices.emplace_back(type);
		m_variableIndices.back().id = m_nextTempId++;
		return int(m_variableIndices.size() - 1);
	}

	CTranslator::VariableIndex CTranslator::popIndex()
	{
		auto top = m_variableIndices.back();
		m_variableIndices.pop_back();
		return top;
	}

	void CTranslator::destroyIndex(const VariableIndex& index)
	{
		if (index.indexType == IndexType::Temporary)
		{
			if (index.type.base == BaseType_String)
			{
				m_writer.indent();
				m_writer << "zrt_str_del(" << index << ");\n";
			}
		}
	}
}
