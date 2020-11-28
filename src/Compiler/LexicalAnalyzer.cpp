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

#include "ZeeBasic/Compiler/LexicalAnalyzer.hpp"

#include "ZeeBasic/Compiler/Error.hpp"

namespace ZeeBasic::Compiler
{

    LexicalAnalyzer::LexicalAnalyzer(ISourceReader& sourceReader)
        :
        m_sourceReader(&sourceReader),
        m_state(State::Begin),
        m_ch(0),
        m_lineNo(0),
        m_colNo(0),
        m_range(),
        m_text()
    { }

    LexicalAnalyzer::~LexicalAnalyzer()
    { }

    Token LexicalAnalyzer::parseNextToken()
    {
        // consume tokens until one should be kept
        m_state = State::Begin;
        do
        {
            m_sourceReader->getReadPosition(m_lineNo, m_colNo);
            if (m_ch == 0)
            {
                m_ch = m_sourceReader->readNextChar();
                if (m_ch == 0)
                {
                    return { TokenId::EndOfCode, {}, {} };
                }
            }

            // prepare for new token
            m_range = { m_lineNo, m_colNo };
            m_text.clear();
            m_text.push_back(m_ch);
            m_state = getTokenStartState(m_ch);
            
            m_ch = m_sourceReader->readNextChar();
            auto result = consumeChar(m_ch);
            while (result == ConsumeState::Consume || result == ConsumeState::ConsumeAndComplete)
            {
                m_text.push_back(m_ch);
                ++m_range;
                m_ch = m_sourceReader->readNextChar();

                if (result == ConsumeState::ConsumeAndComplete)
                {
                    break;
                }

                result = consumeChar(m_ch);
            }
        } while (m_state == State::Whitespace || m_state == State::Comment);

        return constructToken();
    }

    static bool isSymbolStart(char ch)
    {
        auto symbols = "+-*/\\<>=:,;().";
        return strchr(symbols, ch) != nullptr;
    }

    LexicalAnalyzer::State LexicalAnalyzer::getTokenStartState(char ch)
    {
        if (ch == '\t' || ch == ' ')
        {
            return State::Whitespace;
        }
        else if (ch == '\'')
        {
            return State::Comment;
        }
        else if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
        {
            return State::Name;
        }
        else if (ch >= '0' && ch <= '9')
        {
            return State::Integer;
        }
        else if (ch == '.')
        {
            return State::Real;
        }
        else if (ch == '\n')
        {
            return State::EndOfLine;
        }
        else if (ch == '"')
        {
            return State::String;
        }
        else if (isSymbolStart(ch))
        {
            return State::Symbol;
        }

        throw Error::create({ m_lineNo, m_colNo }, "Unexpected character encountered");
    }

    LexicalAnalyzer::ConsumeState LexicalAnalyzer::consumeChar(char ch)
    {
        switch (m_state)
        {

        case State::Whitespace:
            if (m_ch == '\t' || m_ch == ' ')
            {
                return ConsumeState::Consume;
            }
            break;
            
        case State::Comment:
            if (m_ch != '\n' && m_ch != 0)
            {
                return ConsumeState::Consume;
            }
            break;

        case State::Integer:
            if (m_ch == '.')
            {
                m_state = State::Real;
                return ConsumeState::Consume;
            }
            else if (m_ch >= '0' && m_ch <= '9')
            {
                return ConsumeState::Consume;
            }
            break;

        case State::Real:
            if (m_ch >= '0' && m_ch <= '9')
            {
                return ConsumeState::Consume;
            }
            else if (m_text[0] == '.' && m_text.length() == 1)
            {
                m_state = State::Symbol;
            }
            break;

        case State::String:
            if (m_ch == '\n')
            {
                throw Error::create(m_range, "End-of-line not permitted in string literal.");
            }
            else if (m_ch == '"')
            {
                return ConsumeState::ConsumeAndComplete;
            }
            else
            {
                return ConsumeState::Consume;
            }
            break;

        case State::Name:
            if ((m_ch >= 'a' && m_ch <= 'z') ||
                (m_ch >= 'A' && m_ch <= 'Z') ||
                (m_ch >= '0' && m_ch <= '9') ||
                m_ch == '_')
            {
                return ConsumeState::Consume;
            }
            else if (m_ch == '?' || m_ch == '%' || m_ch == '!' || m_ch == '$')
            {
                return ConsumeState::ConsumeAndComplete;
            }
            break;

        case State::Symbol:
            if (m_text[0] == '<' && (m_ch == '=' || m_ch == '>'))
            {
                return ConsumeState::ConsumeAndComplete;
            }
            else if (m_text[0] == '>' && m_ch == '=')
            {
                return ConsumeState::ConsumeAndComplete;
            }
            break;

        case State::EndOfLine:
        default:
            break;
        }

        return ConsumeState::Complete;
    }

    static TokenId getKeywordId(const char* text, int length)
    {
        static struct {
            const char* text;
            int length;
            TokenId id;
        } keywords[] = {
            { "ABS", 3, TokenId::Key_ABS },
            { "AND", 3, TokenId::Key_AND },
            { "AS", 2, TokenId::Key_AS },
            { "ASC", 3, TokenId::Key_ASC },
            { "ATN", 3, TokenId::Key_ATN },
            { "BIN$", 4, TokenId::Key_BIN_S },
            { "BOOLEAN", 7, TokenId::Key_BOOLEAN },
            { "CALL", 4, TokenId::Key_CALL },
            { "CASE", 4, TokenId::Key_CASE },
            { "CHR$", 4, TokenId::Key_CHR_S },
            { "COMMAND$", 8, TokenId::Key_COMMAND_S },
            { "CONST", 5, TokenId::Key_CONST },
            { "COS", 3, TokenId::Key_COS },
            { "DATA", 4, TokenId::Key_DATA },
            { "DATE$", 5, TokenId::Key_DATE_S },
            { "DECLARE", 7, TokenId::Key_DECLARE },
            { "DIM", 3, TokenId::Key_DIM },
            { "DO", 2, TokenId::Key_DO },
            { "ELSE", 4, TokenId::Key_ELSE },
            { "ELSEIF", 6, TokenId::Key_ELSEIF },
            { "END", 3, TokenId::Key_END },
            { "ENVIRON$", 8, TokenId::Key_ENVIRON_S },
            { "EXIT", 4, TokenId::Key_EXIT },
            { "EXP", 3, TokenId::Key_EXP },
            { "FALSE", 5, TokenId::Key_FALSE },
            { "FIX", 3, TokenId::Key_FIX },
            { "FOR", 3, TokenId::Key_FOR },
            { "FUNCTION", 8, TokenId::Key_FUNCTION },
            { "GOSUB", 5, TokenId::Key_GOSUB },
            { "HEX$", 4, TokenId::Key_HEX_S },
            { "IF", 2, TokenId::Key_IF },
            { "INKEY$", 6, TokenId::Key_INKEY_S },
            { "INPUT", 5, TokenId::Key_INPUT },
            { "INSTR", 5, TokenId::Key_INSTR },
            { "INT", 3, TokenId::Key_INT },
            { "INTEGER", 7, TokenId::Key_INTEGER },
            { "IS", 2, TokenId::Key_IS },
            { "LBOUND", 6, TokenId::Key_LBOUND },
            { "LCASE$", 6, TokenId::Key_LCASE_S },
            { "LEFT$", 5, TokenId::Key_LEFT_S },
            { "LEN", 3, TokenId::Key_LEN },
            { "LOG", 3, TokenId::Key_LOG },
            { "LOOP", 4, TokenId::Key_LOOP },
            { "LTRIM$", 6, TokenId::Key_LTRIM_S },
            { "MID$", 4, TokenId::Key_MID_S },
            { "MOD", 3, TokenId::Key_MOD },
            { "NEXT", 4, TokenId::Key_NEXT },
            { "NOT", 3, TokenId::Key_NOT },
            { "OCT$", 4, TokenId::Key_OCT_S },
            { "OR", 2, TokenId::Key_OR },
            { "PRINT", 5, TokenId::Key_PRINT },
            { "RANDOMIZE", 9, TokenId::Key_RANDOMIZE },
            { "READ", 4, TokenId::Key_READ },
            { "REAL", 4, TokenId::Key_REAL },
            { "REDIM", 5, TokenId::Key_REDIM },
            { "RESTORE", 7, TokenId::Key_RESTORE },
            { "RETURN", 6, TokenId::Key_RETURN },
            { "RIGHT$", 6, TokenId::Key_RIGHT_S },
            { "RND", 3, TokenId::Key_RND },
            { "RTRIM$", 6, TokenId::Key_RTRIM_S },
            { "SELECT", 6, TokenId::Key_SELECT },
            { "SHARED", 6, TokenId::Key_SHARED },
            { "SGN", 3, TokenId::Key_SGN },
            { "SIN", 3, TokenId::Key_SIN },
            { "SLEEP", 5, TokenId::Key_SLEEP },
            { "SPACE$", 6, TokenId::Key_SPACE_S },
            { "SQR", 3, TokenId::Key_SQR },
            { "STATIC", 6, TokenId::Key_STATIC },
            { "STEP", 4, TokenId::Key_STEP },
            { "STR$", 4, TokenId::Key_STR_S },
            { "STRING", 6, TokenId::Key_STRING },
            { "STRING$", 7, TokenId::Key_STRING_S },
            { "SUB", 3, TokenId::Key_SUB },
            { "SWAP", 4, TokenId::Key_SWAP },
            { "TAN", 3, TokenId::Key_TAN },
            { "TIME$", 5, TokenId::Key_TIME_S },
            { "TIMER", 5, TokenId::Key_TIMER },
            { "THEN", 4, TokenId::Key_THEN },
            { "TO", 2, TokenId::Key_TO },
            { "TRUE", 4, TokenId::Key_TRUE },
            { "TYPE", 4, TokenId::Key_TYPE },
            { "UBOUND", 6, TokenId::Key_UBOUND },
            { "UCASE$", 6, TokenId::Key_UCASE_S },
            { "UNTIL", 5, TokenId::Key_UNTIL },
            { "VAL", 3, TokenId::Key_VAL },
            { "WHILE", 5, TokenId::Key_WHILE },
            { "XOR", 3, TokenId::Key_XOR },
            { nullptr, 0, TokenId::EndOfCode }
        };
        for (auto i = 0; keywords[i].text; ++i)
        {
#ifdef _WIN32
            if (keywords[i].length == length && _stricmp(keywords[i].text, text) == 0)
#else
            if (keywords[i].length == length && strcasecmp(keywords[i].text, text) == 0)
#endif
            {
                return keywords[i].id;
            }
        }

        return TokenId::UntypedName;
    }

    static TokenId matchSymbolId(const char* text)
    {
        static struct {
            const char* symbol;
            TokenId id;
        } symbols[] = {
            { "+", TokenId::Sym_Add },
            { "-", TokenId::Sym_Subtract },
            { "*", TokenId::Sym_Multiply },
            { "/", TokenId::Sym_Divide },
            { "\\", TokenId::Sym_IntDivide },
            { "<", TokenId::Sym_Less },
            { "<=", TokenId::Sym_LessEquals },
            { ">", TokenId::Sym_Greater },
            { ">=", TokenId::Sym_GreaterEquals },
            { "=", TokenId::Sym_Equal },
            { "<>", TokenId::Sym_NotEqual },
            { ":", TokenId::Sym_Colon },
            { ",", TokenId::Sym_Comma },
            { ";", TokenId::Sym_Semicolon },
            { "(", TokenId::Sym_OpenParen },
            { ")", TokenId::Sym_CloseParen },
            { ".", TokenId::Sym_Period},
            { nullptr, TokenId::EndOfCode }
        };
        for (auto i = 0; symbols[i].symbol; ++i)
        {
            if (strcmp(symbols[i].symbol, text) == 0)
            {
                return symbols[i].id;
            }
        }

        throw std::runtime_error("Internal state error");
    }

    Token LexicalAnalyzer::constructToken()
    {
        switch (m_state)
        {

        case State::Integer:
            return { TokenId::Integer, m_range, { m_text.c_str(), int(m_text.length()) } };

        case State::Real:
            return { TokenId::Real, m_range, { m_text.c_str(), int(m_text.length()) } };

        case State::String:
            return { TokenId::String, m_range, { m_text.c_str() + 1, int(m_text.length()) - 2 } };

        case State::Name:
            // check for keyword first
            if (auto id = getKeywordId(m_text.c_str(), int(m_text.length())); id != TokenId::UntypedName)
            {
                return { id, m_range, { m_text.c_str(), int(m_text.length()) } };
            }

            if (auto ch = m_text[m_text.length() - 1]; ch == '?' || ch == '%' || ch == '!' || ch == '$')
            {
                return { TokenId::TypedName, m_range, { m_text.c_str(), int(m_text.length()) } };
            }

            return { TokenId::UntypedName, m_range, { m_text.c_str(), int(m_text.length()) } };

        case State::Symbol:
            return { matchSymbolId(m_text.c_str()), m_range, { m_text.c_str(), int(m_text.length()) } };

        case State::EndOfLine:
            return { TokenId::EndOfLine, m_range, {} };

        default:
            break;
        }

        throw std::runtime_error("Internal state error");
    }

}
