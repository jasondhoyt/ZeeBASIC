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

#include "ZeeBasic/Compiler/IntegerLiteralNode.hpp"

namespace ZeeBasic::Compiler::Nodes
{

	std::unique_ptr<ExpressionNode> ExpressionNode::parseExpression(IParser& parser)
	{
		auto& token = parser.getToken();

		auto lhs = std::unique_ptr<ExpressionNode>{};
		switch (token.id)
		{
		case TokenId::Integer:
			lhs = std::make_unique<IntegerLiteralNode>();
			break;
		default:
			return {};
		}

		assert(lhs);
		lhs->parse(parser);


		return lhs;
	}

	ExpressionNode::ExpressionNode()
		:
		Node(),
		m_type(BaseType_Unknown)
	{ }

	ExpressionNode::~ExpressionNode()
	{ }

}