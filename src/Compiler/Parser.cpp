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

#include "ZeeBasic/Compiler/Parser.hpp"

#include "ZeeBasic/Compiler/Error.hpp"
#include "ZeeBasic/Compiler/LexicalAnalyzer.hpp"
#include "ZeeBasic/Compiler/Node.hpp"
#include "ZeeBasic/Compiler/Program.hpp"
#include "ZeeBasic/Compiler/StatementNode.hpp"

namespace ZeeBasic::Compiler
{

	Parser::Parser(ISourceReader& source, Program& program)
		:
		m_lexicalAnalyzer(source),
		m_program(program),
		m_tokens()
	{ }

	Parser::~Parser()
	{ }

	void Parser::run()
	{
		auto stm = Nodes::parseStatement(*this);
		while (stm)
		{
			m_program.statements.push_back(std::move(stm));
			stm = Nodes::parseStatement(*this);
		}
	}

	const Token& Parser::getToken(int lookAhead)
	{
		while (m_tokens.size() < size_t(lookAhead) + 1)
		{
			m_tokens.push_back(m_lexicalAnalyzer.parseNextToken());
		}

		return m_tokens[lookAhead];
	}

	static const char* getExpectedTokenString(TokenId id)
	{
		switch (id)
		{
		case TokenId::Integer: return "integer literal";
		default:
			break;
		}

		return "????";
	}

	const Token& Parser::expectToken(TokenId id)
	{
		auto& token = getToken();
		if (token.id != id)
		{
			throw Error::create(token.range, "Expected %s", getExpectedTokenString(id));
		}

		return token;
	}

	void Parser::eatToken()
	{
		if (m_tokens.size() > 1)
		{
			m_tokens.erase(m_tokens.begin());
			return;
		}

		m_tokens[0] = m_lexicalAnalyzer.parseNextToken();
	}

	void Parser::eatEndOfLine()
	{
		auto& token = getToken();
		if (token.id != TokenId::EndOfLine && token.id != TokenId::Sym_Colon)
		{
			throw Error::create(token.range, "Expected end-of-line");
		}

		eatToken();
	}

}
