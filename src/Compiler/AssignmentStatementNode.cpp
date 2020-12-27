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

#include "ZeeBasic/Compiler/AssignmentStatementNode.hpp"

#include "ZeeBasic/Compiler/Error.hpp"
#include "ZeeBasic/Compiler/ExpressionNode.hpp"
#include "ZeeBasic/Compiler/SymbolTable.hpp"

namespace ZeeBasic::Compiler::Nodes
{

	void AssignmentStatementNode::parse(IParser& parser)
	{
		auto& token = parser.getToken();
		auto& name = token.text;

		auto type = Type{};
		if (name.endsWith('$'))
		{
			type.base = BaseType_String;
		}
		else
		{
			type.base = BaseType_Integer;
		}
		
		m_symbol = parser.getSymbolTable().findOrCreateSymbol(name, token.range, type);

		parser.eatToken();
		if (parser.getToken().id != TokenId::Sym_Equal)
		{
			throw Error::create(parser.getToken().range, "Expected equals after variable name for assignment");
		}
		m_range = parser.getToken().range;
		parser.eatToken();

		m_expr = ExpressionNode::parseExpression(parser);
		if (!m_expr)
		{
			throw Error::create(parser.getToken().range, "Expected expression for assignment");
		}

		parser.eatEndOfLine();
	}

	void AssignmentStatementNode::analyze(IAnalyzer& analyzer)
	{
		if (m_expr)
		{
			m_expr->analyze(analyzer);
		}

		// TODO : implicit type conversions (int-based)

		if (m_expr->getType().base != m_symbol->type.base)
		{
			throw Error::create(m_range, "Unable to implicitly cast type");
		}
	}

	void AssignmentStatementNode::translate(ITranslator& translator)
	{
		translator.translate(*this);
	}

}