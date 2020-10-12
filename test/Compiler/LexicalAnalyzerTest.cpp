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

#include <gtest/gtest.h>

#include <vector>

#include "ZeeBasic/Compiler/LexicalAnalyzer.hpp"

#include "ZeeBasic/Compiler/Error.hpp"

using namespace ZeeBasic::Compiler;

class StringSourceReader
    :
    public ISourceReader
{
public:
    StringSourceReader(const char* code)
        :
        ISourceReader(),
        m_lineNo(1),
        m_colNo(1),
        m_text(code),
        m_offset(0)
    { }

    ~StringSourceReader()
    { }

    void getReadPosition(int& lineNo, int& colNo) override { lineNo = m_lineNo; colNo = m_colNo; }

    char readNextChar() override
    {
        if (m_text[m_offset] == 0)
        {
            return 0;
        }

        if (m_text[m_offset] == '\n')
        {
            ++m_lineNo;
            m_colNo = 1;
        }
        else
        {
            m_colNo++;
        }

        return m_text[m_offset++];
    }

private:
    int m_lineNo;
    int m_colNo;
    const char* m_text;
    int m_offset;
};

std::vector<Token> parseIntoTokens(const char* code)
{
    auto reader = StringSourceReader{ code };
    auto lexicalAnalyzer = LexicalAnalyzer{ reader };
    auto tokens = std::vector<Token>{};
    do {
        auto token = lexicalAnalyzer.parseNextToken();
        tokens.push_back(token);
        if (token.id == TokenId::EndOfCode)
        {
            break;
        }        
    } while (true);

    return tokens;
}

Token parseIntoToken(const char* code)
{
    auto reader = StringSourceReader{ code };
    return LexicalAnalyzer{ reader }.parseNextToken();
}

TEST(ZeeBasic_Compiler_LexicalAnalyzer, ParseEndOfCode)
{
    auto token = parseIntoToken("");
    EXPECT_EQ(token.id, TokenId::EndOfCode);
    EXPECT_EQ(token.text, "");

    auto tokens = parseIntoTokens("23");
    EXPECT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].id, TokenId::Integer);
    EXPECT_EQ(tokens[1].id, TokenId::EndOfCode);

    tokens = parseIntoTokens("  ");
    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].id, TokenId::EndOfCode);
}

TEST(ZeeBasic_Compiler_LexicalAnalyzer, Whitespace)
{
    auto tokens = parseIntoTokens("  \t\t  23\t  ");
    EXPECT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].id, TokenId::Integer);
    EXPECT_EQ(tokens[1].id, TokenId::EndOfCode);

    auto token = parseIntoToken("    ");
    EXPECT_EQ(token.id, TokenId::EndOfCode);

    token = parseIntoToken("\t\t\t");
    EXPECT_EQ(token.id, TokenId::EndOfCode);
}

TEST(ZeeBasic_Compiler_LexicalAnalyzer, Comment)
{
    auto token = parseIntoToken("'");
    EXPECT_EQ(token.id, TokenId::EndOfCode);

    token = parseIntoToken("'23");
    EXPECT_EQ(token.id, TokenId::EndOfCode);

    token = parseIntoToken("' this is a test  ");
    EXPECT_EQ(token.id, TokenId::EndOfCode);

    auto tokens = parseIntoTokens("' comment 1\n'comment 2");
    EXPECT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].id, TokenId::EndOfLine);
    EXPECT_EQ(tokens[1].id, TokenId::EndOfCode);
}

TEST(ZeeBasic_Compiler_LexicalAnalyzer, EndOfLine)
{
    auto tokens = parseIntoTokens("\n");
    EXPECT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].id, TokenId::EndOfLine);
    EXPECT_EQ(tokens[1].id, TokenId::EndOfCode);

    tokens = parseIntoTokens("\n  \n\n  \t\t\n");
    EXPECT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0].id, TokenId::EndOfLine);
    EXPECT_EQ(tokens[1].id, TokenId::EndOfLine);
    EXPECT_EQ(tokens[2].id, TokenId::EndOfLine);
    EXPECT_EQ(tokens[3].id, TokenId::EndOfLine);
    EXPECT_EQ(tokens[4].id, TokenId::EndOfCode);
}

TEST(ZeeBasic_Compiler_LexicalAnalyzer, InvalidCharacter)
{
    EXPECT_THROW(parseIntoTokens("~"), Error);
    EXPECT_THROW(parseIntoTokens("ab~"), Error);
}

TEST(ZeeBasic_Compiler_LexicalAnalyzer, String)
{
    auto token = parseIntoToken("\"\"");
    EXPECT_EQ(token.id, TokenId::String);
    EXPECT_EQ(token.text, "");

    token = parseIntoToken("\"test\"");
    EXPECT_EQ(token.id, TokenId::String);
    EXPECT_EQ(token.text, "test");

    token = parseIntoToken("\"  This has\t\t whitespace\"");
    EXPECT_EQ(token.id, TokenId::String);
    EXPECT_EQ(token.text, "  This has\t\t whitespace");

    EXPECT_THROW(parseIntoToken("\"unterminated string\n\""), Error);
}

TEST(ZeeBasic_Compiler_LexicalAnalyzer, IntegerReal)
{
    static struct {
        const char* code;
        TokenId id;
    } tests[] = {
        { "0", TokenId::Integer },
        { "42", TokenId::Integer },
        { ".1", TokenId::Real },
        { "1.0", TokenId::Real },
        { "25.", TokenId::Real },
        { nullptr, TokenId::EndOfCode }
    };
    for (auto i = 0; tests[i].code; ++i)
    {
        auto token = parseIntoToken(tests[i].code);
        EXPECT_EQ(token.id, tests[i].id);
        EXPECT_EQ(token.text, tests[i].code);
    }
}

TEST(ZeeBasic_Compiler_LexicalAnalyzer, Name)
{
    auto token = parseIntoToken("test");
    EXPECT_EQ(token.id, TokenId::UntypedName);
    EXPECT_EQ(token.text, "test");

    token = parseIntoToken("strVar$");
    EXPECT_EQ(token.id, TokenId::TypedName);
    EXPECT_EQ(token.text, "strVar$");

    token = parseIntoToken("boolVar?");
    EXPECT_EQ(token.id, TokenId::TypedName);
    EXPECT_EQ(token.text, "boolVar?");

    token = parseIntoToken("intVar%");
    EXPECT_EQ(token.id, TokenId::TypedName);
    EXPECT_EQ(token.text, "intVar%");

    token = parseIntoToken("realVar!");
    EXPECT_EQ(token.id, TokenId::TypedName);
    EXPECT_EQ(token.text, "realVar!");

    EXPECT_THROW(parseIntoTokens("boolVar??"), Error);
    EXPECT_THROW(parseIntoTokens("intVar%%"), Error);
    EXPECT_THROW(parseIntoTokens("realVar!!"), Error);
    EXPECT_THROW(parseIntoTokens("strVar$$"), Error);
}

TEST(ZeeBasic_Compiler_LexicalAnalyzer, Keywords)
{
static struct {
        const char* text;
        TokenId id;
    } keywords[] = {
        { "ABS", TokenId::Key_ABS },
        { "AND", TokenId::Key_AND },
        { "AS", TokenId::Key_AS },
        { "ASC", TokenId::Key_ASC },
        { "ATN", TokenId::Key_ATN },
        { "BIN$", TokenId::Key_BIN_S },
        { "BOOLEAN", TokenId::Key_BOOLEAN },
        { "CALL", TokenId::Key_CALL },
        { "CASE", TokenId::Key_CASE },
        { "CHR$", TokenId::Key_CHR_S },
        { "COMMAND$", TokenId::Key_COMMAND_S },
        { "CONST", TokenId::Key_CONST },
        { "COS", TokenId::Key_COS },
        { "DATA", TokenId::Key_DATA },
        { "DATE$", TokenId::Key_DATE_S },
        { "DECLARE", TokenId::Key_DECLARE },
        { "DIM", TokenId::Key_DIM },
        { "DO", TokenId::Key_DO },
        { "ELSE", TokenId::Key_ELSE },
        { "ELSEIF", TokenId::Key_ELSEIF },
        { "END", TokenId::Key_END },
        { "ENVIRON$", TokenId::Key_ENVIRON_S },
        { "EXIT", TokenId::Key_EXIT },
        { "EXP", TokenId::Key_EXP },
        { "FALSE", TokenId::Key_FALSE },
        { "FIX", TokenId::Key_FIX },
        { "FOR", TokenId::Key_FOR },
        { "FUNCTION", TokenId::Key_FUNCTION },
        { "GOSUB", TokenId::Key_GOSUB },
        { "HEX$", TokenId::Key_HEX_S },
        { "IF", TokenId::Key_IF },
        { "INKEY$", TokenId::Key_INKEY_S },
        { "INPUT", TokenId::Key_INPUT },
        { "INSTR", TokenId::Key_INSTR },
        { "INT", TokenId::Key_INT },
        { "INTEGER", TokenId::Key_INTEGER },
        { "IS", TokenId::Key_IS },
        { "LBOUND", TokenId::Key_LBOUND },
        { "LCASE$", TokenId::Key_LCASE_S },
        { "LEFT$", TokenId::Key_LEFT_S },
        { "LEN", TokenId::Key_LEN },
        { "LOG", TokenId::Key_LOG },
        { "LOOP", TokenId::Key_LOOP },
        { "LTRIM$", TokenId::Key_LTRIM_S },
        { "MID$", TokenId::Key_MID_S },
        { "MOD", TokenId::Key_MOD },
        { "NEXT", TokenId::Key_NEXT },
        { "NOT", TokenId::Key_NOT },
        { "OCT$", TokenId::Key_OCT_S },
        { "OR", TokenId::Key_OR },
        { "PRINT", TokenId::Key_PRINT },
        { "RANDOMIZE", TokenId::Key_RANDOMIZE },
        { "READ", TokenId::Key_READ },
        { "REAL", TokenId::Key_REAL },
        { "REDIM", TokenId::Key_REDIM },
        { "RESTORE", TokenId::Key_RESTORE },
        { "RETURN", TokenId::Key_RETURN },
        { "RIGHT$", TokenId::Key_RIGHT_S },
        { "RND", TokenId::Key_RND },
        { "RTRIM$", TokenId::Key_RTRIM_S },
        { "SELECT", TokenId::Key_SELECT },
        { "SHARED", TokenId::Key_SHARED },
        { "SGN", TokenId::Key_SGN },
        { "SIN", TokenId::Key_SIN },
        { "SLEEP", TokenId::Key_SLEEP },
        { "SPACE$", TokenId::Key_SPACE_S },
        { "SQR", TokenId::Key_SQR },
        { "STATIC", TokenId::Key_STATIC },
        { "STEP", TokenId::Key_STEP },
        { "STR$", TokenId::Key_STR_S },
        { "STRING", TokenId::Key_STRING },
        { "STRING$", TokenId::Key_STRING_S },
        { "SUB", TokenId::Key_SUB },
        { "SWAP", TokenId::Key_SWAP },
        { "TAN", TokenId::Key_TAN },
        { "TIME$", TokenId::Key_TIME_S },
        { "TIMER", TokenId::Key_TIMER },
        { "THEN", TokenId::Key_THEN },
        { "TO", TokenId::Key_TO },
        { "TRUE", TokenId::Key_TRUE },
        { "TYPE", TokenId::Key_TYPE },
        { "UBOUND", TokenId::Key_UBOUND },
        { "UCASE$", TokenId::Key_UCASE_S },
        { "UNTIL", TokenId::Key_UNTIL },
        { "VAL", TokenId::Key_VAL },
        { "WHILE", TokenId::Key_WHILE },
        { "XOR", TokenId::Key_XOR },
        { nullptr, TokenId::EndOfCode }
    };
    for (auto i = 0; keywords[i].text; ++i)
    {
        auto tokens = parseIntoTokens(keywords[i].text);
        EXPECT_EQ(tokens.size(), 2);
        EXPECT_EQ(tokens[0].id, keywords[i].id);
        EXPECT_EQ(tokens[0].text, keywords[i].text);
        EXPECT_EQ(tokens[1].id, TokenId::EndOfCode);
    }
}

TEST(ZeeBasic_Compiler_LexicalAnalyzer, Symbols)
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
        auto tokens = parseIntoTokens(symbols[i].symbol);
        EXPECT_EQ(tokens.size(), 2);
        EXPECT_EQ(tokens[0].id, symbols[i].id);
        EXPECT_EQ(tokens[0].text, symbols[i].symbol);
        EXPECT_EQ(tokens[1].id, TokenId::EndOfCode);
    }
}
