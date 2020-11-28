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

#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "ZeeBasic/Compiler/FileSourceReader.hpp"

using namespace ZeeBasic::Compiler;

TEST(ZeeBasic_Compiler_FileSourceReader, Initialization)
{
    EXPECT_THROW(FileSourceReader{ "bad.file" }, std::runtime_error);

    std::ofstream{ "test.zee" } << "Test File";
    EXPECT_NO_THROW(FileSourceReader{ "test.zee" });
    std::filesystem::remove("test.zee");
}

TEST(ZeeBasic_Compiler_FileSourceReader, ReadData)
{
    std::ofstream{ "test2.zee" } << "File\nWith\nNewlines";
    auto reader = FileSourceReader{ "test2.zee" };

    auto lineNo = 0;
    auto colNo = 0;
    reader.getReadPosition(lineNo, colNo);
    EXPECT_EQ(lineNo, 1);
    EXPECT_EQ(colNo, 1);

    auto ch = reader.readNextChar();
    EXPECT_EQ(ch, 'F');
    reader.getReadPosition(lineNo, colNo);
    EXPECT_EQ(lineNo, 1);
    EXPECT_EQ(colNo, 2);

    ch = reader.readNextChar();
    EXPECT_EQ(ch, 'i');
    ch = reader.readNextChar();
    EXPECT_EQ(ch, 'l');
    ch = reader.readNextChar();
    EXPECT_EQ(ch, 'e');
    reader.getReadPosition(lineNo, colNo);
    EXPECT_EQ(lineNo, 1);
    EXPECT_EQ(colNo, 5);

    ch = reader.readNextChar();
    EXPECT_EQ(ch, '\n');
    reader.getReadPosition(lineNo, colNo);
    EXPECT_EQ(lineNo, 2);
    EXPECT_EQ(colNo, 1);

    for (auto i = 0; i < 12; ++i)
    {
        (void)reader.readNextChar();
    }

    reader.getReadPosition(lineNo, colNo);
    EXPECT_EQ(lineNo, 3);
    EXPECT_EQ(colNo, 8);

    ch = reader.readNextChar();
    EXPECT_EQ(ch, 's');
    reader.getReadPosition(lineNo, colNo);
    EXPECT_EQ(lineNo, 3);
    EXPECT_EQ(colNo, 9);

    for (auto i = 0; i < 3; ++i)
    {
        ch = reader.readNextChar();
        EXPECT_EQ(ch, 0);
        reader.getReadPosition(lineNo, colNo);
        EXPECT_EQ(lineNo, 3);
        EXPECT_EQ(colNo, 9);
    }

    std::filesystem::remove("test.zee");
}
