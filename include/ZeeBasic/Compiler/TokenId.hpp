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
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

namespace ZeeBasic::Compiler
{

    enum class TokenId
    {
        // marks end of all code
        EndOfCode,

        // marks newline
        EndOfLine,

        // normally ignored
        Whitespace,
        Comment,

        // literals
        Integer,
        Real,
        String,

        // user-defined names
        UntypedName,
        TypedName,

        // keywords
        Key_ABS, Key_AND, Key_AS, Key_ASC, Key_ATN,
        Key_BIN_S, Key_BOOLEAN,
        Key_CALL, Key_CASE, Key_CHR_S, Key_COMMAND_S, Key_CONST, Key_COS,
        Key_DATA, Key_DATE_S, Key_DECLARE, Key_DIM, Key_DO,
        Key_ELSE, Key_ELSEIF, Key_END, Key_ENVIRON_S, Key_EXIT, Key_EXP,
        Key_FALSE, Key_FIX, Key_FOR, Key_FUNCTION, 
        Key_GOSUB,
        Key_HEX_S,
        Key_IF, Key_INKEY_S, Key_INPUT, Key_INSTR, Key_INT, Key_INTEGER, Key_IS,
        Key_LBOUND, Key_LCASE_S, Key_LEFT_S, Key_LEN, Key_LOG, Key_LOOP, Key_LTRIM_S,
        Key_MID_S, Key_MOD,
        Key_NEXT, Key_NOT,
        Key_OCT_S, Key_OR,
        Key_PRINT,
        Key_RANDOMIZE, Key_READ, Key_REAL, Key_REDIM, Key_RESTORE, Key_RETURN, Key_RIGHT_S, Key_RND, Key_RTRIM_S,
        Key_SELECT, Key_SHARED, Key_SGN, Key_SIN, Key_SLEEP, Key_SPACE_S, Key_SQR, Key_STATIC, Key_STEP,
            Key_STR_S, Key_STRING, Key_STRING_S, Key_SUB, Key_SWAP,
        Key_TAN, Key_TIME_S, Key_TIMER, Key_THEN, Key_TO, Key_TRUE, Key_TYPE,
        Key_UBOUND, Key_UCASE_S, Key_UNTIL,
        Key_VAL,
        Key_WHILE,
        Key_XOR,

        // symbols
        Sym_Add, Sym_Subtract,
        Sym_Multiply, Sym_Divide, Sym_IntDivide,
        Sym_Less, Sym_LessEquals, Sym_Greater, Sym_GreaterEquals,
        Sym_Equal, Sym_NotEqual,
        Sym_Colon,
        Sym_Comma,
        Sym_Semicolon,
        Sym_OpenParen,
        Sym_CloseParen,
        Sym_Period,
    };

}
