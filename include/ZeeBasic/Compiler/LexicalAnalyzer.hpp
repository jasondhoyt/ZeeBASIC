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

#pragma once

#include <string>

#include "ISourceReader.hpp"
#include "Token.hpp"

namespace ZeeBasic::Compiler
{

    // Class that handles parsing source code into a stream of tokens for the compiler.
    class LexicalAnalyzer
    {
    public:
        LexicalAnalyzer(ISourceReader& sourceReader);
        ~LexicalAnalyzer();

        // Returns a token with an id of TokenId::EndOfCode when there is no more code to parse.
        Token parseNextToken();

    private:
        ISourceReader* m_sourceReader;

        enum class State
        {
            Begin,
            Whitespace,
            Comment,
            Integer,
            Real,
            String,
            Name,
            Symbol,
            EndOfLine,
        };
        State m_state;

        // current character being analyzed
        char m_ch;
        int m_lineNo;
        int m_colNo;

        // start of current token being parsed
        Range m_range;

        // contents of token text;
        std::string m_text;

        // determine what type of token the character starts
        State getTokenStartState(char ch);

        // consume next character if possible for the current token being built
        enum class ConsumeState
        {
            Consume,
            Complete,
            ConsumeAndComplete
        };
        ConsumeState consumeChar(char ch);

        // construct a token from current parsed data
        Token constructToken();
    };

}
