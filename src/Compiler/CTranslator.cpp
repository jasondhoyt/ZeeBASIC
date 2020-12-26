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

#include "ZeeBasic/Compiler/BinaryExpressionNode.hpp"
#include "ZeeBasic/Compiler/IntegerLiteralNode.hpp"
#include "ZeeBasic/Compiler/PrintStatementNode.hpp"
#include "ZeeBasic/Compiler/Program.hpp"

namespace ZeeBasic::Compiler
{

	CTranslator::CTranslator(const std::string& path, const Program& program)
		:
		ITranslator(),
		m_file(nullptr),
		m_program(program),
		m_temps(),
		m_nextTempId(1)
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

		m_indent = 4;

		for (const auto& stm : m_program.statements)
		{
			stm->translate(*this);
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

	void CTranslator::translate(const Nodes::BinaryExpressionNode& node)
	{
		node.getLeft().translate(*this);
		node.getRight().translate(*this);

		auto rhs = m_temps.top();
		m_temps.pop();

		auto lhs = m_temps.top();
		m_temps.pop();

		auto id = makeTemp(BaseType_Integer);

		indent();
		switch (node.getOperator())
		{

		case Nodes::BinaryExpressionNode::Operator::Add:
			fprintf(m_file, "zrt_Int t_%d = t_%d + t_%d;\n", id, lhs.id, rhs.id);
			break;

		case Nodes::BinaryExpressionNode::Operator::Subtract:
			fprintf(m_file, "zrt_Int t_%d = t_%d - t_%d;\n", id, lhs.id, rhs.id);
			break;

		case Nodes::BinaryExpressionNode::Operator::Multiply:
			fprintf(m_file, "zrt_Int t_%d = t_%d * t_%d;\n", id, lhs.id, rhs.id);
			break;

		case Nodes::BinaryExpressionNode::Operator::Divide:
			fprintf(m_file, "zrt_Int t_%d = t_%d / t_%d;\n", id, lhs.id, rhs.id);
			break;

		default:
			assert(false);

		}
		
	}

	void CTranslator::translate(const Nodes::IntegerLiteralNode& node)
	{
		auto value = node.getValue();
		auto id = makeTemp(BaseType_Integer);

		indent();
		fprintf(m_file, "zrt_Int t_%d = %lld;\n", id, value);
	}

	void CTranslator::translate(const Nodes::PrintStatementNode& node)
	{
		const auto expr = node.getExpression();
		if (expr)
		{
			expr->translate(*this);
			auto temp = m_temps.top();
			m_temps.pop();

			indent();

			switch (temp.type.base)
			{
			case BaseType_Integer:
				fprintf(m_file, "zrt_println_int(t_%d);\n", temp.id);
				break;
			default:
				assert(false);
				break;
			}

			destroyTemp(temp);
		}
		else
		{
			indent();
			fprintf(m_file, "zrt_println();\n");
		}
	}

	void CTranslator::indent()
	{
		for (int i = 0; i < m_indent; ++i)
		{
			fputc(' ', m_file);
		}
	}

	int CTranslator::makeTemp(Type type)
	{
		auto id = m_nextTempId++;

		m_temps.emplace(type, id);

		return id;
	}

	void CTranslator::destroyTemp(const Temporary& temp)
	{
		// TODO
	}
}
