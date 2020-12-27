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

#include "ZeeBasic/Compiler/FunctionCallExpressionNode.hpp"

#include "ZeeBasic/Compiler/Error.hpp"

namespace ZeeBasic::Compiler::Nodes
{

	void FunctionCallExpressionNode::parse(IParser& parser)
	{
		auto& token = parser.getToken();
		m_name = token.text;
		// TODO
		m_range = token.range;
		parser.eatToken();

		if (parser.getToken().id == TokenId::Sym_OpenParen)
		{
			parser.eatToken();

			while (true)
			{
				auto expr = parseExpression(parser);
				if (!expr)
				{
					throw Error::create(parser.getToken().range, "Expected argument for function call");
				}

				m_arguments.emplace_back(std::move(expr));

				if (parser.getToken().id != TokenId::Sym_Comma)
				{
					break;
				}

				parser.eatToken();
			}

			parser.expectToken(TokenId::Sym_CloseParen);
			parser.eatToken();
		}
	}

	void FunctionCallExpressionNode::analyze(IAnalyzer& analyzer)
	{
		for (auto& arg : m_arguments)
		{
			arg->analyze(analyzer);
		}

		// TODO : better lookup for built-in function types
		if (m_name == "STR$")
		{
			m_type = Type{ BaseType_String };
			if (m_arguments.size() != 1)
			{
				throw Error::create(m_range, "Bad arguments for built-in function");
			}

			if (m_arguments[0]->getType().base != BaseType_Integer)
			{
				throw Error::create(m_range, "Expected integer argument for built-in function");
			}
		}
		else
		{
			assert(false);
		}
	}

	void FunctionCallExpressionNode::translate(ITranslator& translator)
	{
		translator.translate(*this);
	}

}
