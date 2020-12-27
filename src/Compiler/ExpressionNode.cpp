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

#include "ZeeBasic/Compiler/ExpressionNode.hpp"

#include "ZeeBasic/Compiler/BinaryExpressionNode.hpp"
#include "ZeeBasic/Compiler/Error.hpp"
#include "ZeeBasic/Compiler/FunctionCallExpressionNode.hpp"
#include "ZeeBasic/Compiler/IdentifierExpressionNode.hpp"
#include "ZeeBasic/Compiler/IntegerLiteralNode.hpp"
#include "ZeeBasic/Compiler/StringLiteralNode.hpp"

namespace ZeeBasic::Compiler::Nodes
{

	static int getPrecedence(const Token& token)
	{
		switch (token.id)
		{
		case TokenId::Sym_Add:
		case TokenId::Sym_Subtract:
			return 7;

		case TokenId::Sym_Multiply:
		case TokenId::Sym_Divide:
		case TokenId::Sym_IntDivide:
			return 8;

		default:
			break;
		}

		return 0;
	}

	static BinaryExpressionNode::Operator mapBinaryOperator(TokenId id)
	{
		switch (id)
		{
		case TokenId::Sym_Add: return BinaryExpressionNode::Operator::Add;
		case TokenId::Sym_Subtract: return BinaryExpressionNode::Operator::Subtract;
		case TokenId::Sym_Multiply: return BinaryExpressionNode::Operator::Multiply;
		case TokenId::Sym_Divide: return BinaryExpressionNode::Operator::Divide;
		case TokenId::Sym_IntDivide: return BinaryExpressionNode::Operator::IntDivide;
		default:
			break;
		}

		assert(false);
		return (BinaryExpressionNode::Operator)0;
	}

	static bool isBuiltinFunction(TokenId id)
	{
		switch (id)
		{

		case TokenId::Key_STR_S:
			return true;

		default:
			break;

		}

		return false;
	}

	static std::unique_ptr<ExpressionNode> parseSubExpression(IParser& parser, int prec)
	{
		auto& token = parser.getToken();

		auto lhs = std::unique_ptr<ExpressionNode>{};
		switch (token.id)
		{

		case TokenId::Integer:
			lhs = std::make_unique<IntegerLiteralNode>();
			break;

		case TokenId::String:
			lhs = std::make_unique<StringLiteralNode>();
			break;

		case TokenId::Sym_OpenParen:
			parser.eatToken();
			lhs = parseSubExpression(parser, 0);
			parser.expectToken(TokenId::Sym_CloseParen);
			parser.eatToken();
			break;

		case TokenId::TypedName:
		case TokenId::UntypedName:
			// TODO : user-defined function
			lhs = std::make_unique<IdentifierExpressionNode>();
			break;

		default:
			if (isBuiltinFunction(token.id))
			{
				lhs = std::make_unique<FunctionCallExpressionNode>();
			}
			else
			{
				return {};
			}

		}

		assert(lhs);
		lhs->parse(parser);

		while (true)
		{
			auto newPrec = getPrecedence(parser.getToken());
			if (prec >= newPrec)
				break;

			auto id = parser.getToken().id;
			parser.eatToken();

			auto rhs = parseSubExpression(parser, newPrec);
			if (!rhs)
			{
				throw Error::create(parser.getToken().range, "Expected expression for right-hand side of operator");
			}

			lhs = std::make_unique<BinaryExpressionNode>(mapBinaryOperator(id), std::move(lhs), std::move(rhs));
			lhs->parse(parser);
		}

		return lhs;
	}

	std::unique_ptr<ExpressionNode> ExpressionNode::parseExpression(IParser& parser)
	{
		return parseSubExpression(parser, 0);
	}

	ExpressionNode::ExpressionNode()
		:
		Node(),
		m_type(BaseType_Unknown)
	{ }

	ExpressionNode::~ExpressionNode()
	{ }

}
