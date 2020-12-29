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
#include <cstdlib>

#include "ZeeBasic/Compiler/UnaryExpressionNode.hpp"

#include "ZeeBasic/Compiler/Error.hpp"

namespace ZeeBasic::Compiler::Nodes
{

	void UnaryExpressionNode::parse(IParser& parser)
	{
		auto& token = parser.getToken();
		if (token.id == TokenId::Sym_Subtract)
		{
			m_op = Operator::Negate;
		}
		else if (token.id == TokenId::Key_NOT)
		{
			m_op = Operator::BitwiseNot;
		}
		m_range = token.range;

		parser.eatToken();

		m_expr = ExpressionNode::parseExpression(parser, 10);
		if (!m_expr)
		{
			throw Error::create(parser.getToken().range, "Expected expression after unary operator");
		}

		switch (m_expr->getType().base)
		{

		case BaseType_Boolean:
			if (m_op == Operator::Negate)
			{
				throw Error::create(m_range, "Operator not allowed for boolean type.");
			}
			break;

		case BaseType_Integer:
			// both allowed
			break;

		case BaseType_Real:
			if (m_op == Operator::BitwiseNot)
			{
				throw Error::create(m_range, "Operator not allowed for real type.");
			}
			break;

		case BaseType_String:
			throw Error::create(m_range, "Operator not allowed for string type.");

		default:
			assert(false);
			break;

		}

		m_type = m_expr->getType();
	}

	void UnaryExpressionNode::translate(ITranslator& translator) const
	{
		translator.translate(*this);
	}

}
