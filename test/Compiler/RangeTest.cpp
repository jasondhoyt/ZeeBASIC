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

#include "ZeeBasic/Compiler/Range.hpp"

using namespace ZeeBasic::Compiler;

TEST(ZeeBasic_Compiler_Range, Initialization)
{
    auto range = Range{};
    EXPECT_EQ(range.startCol, 0);
    EXPECT_EQ(range.endCol, 0);
    EXPECT_EQ(range.startLine, 0);
    EXPECT_EQ(range.endLine, 0);

    auto range2 = Range{ 1, 2 };
    EXPECT_EQ(range2.startCol, 2);
    EXPECT_EQ(range2.endCol, 2);
    EXPECT_EQ(range2.startLine, 1);
    EXPECT_EQ(range2.endLine, 1);
}

TEST(ZeeBasic_Compiler_Range, Add)
{
    auto range = Range{ 1, 1 };
    EXPECT_EQ(range.startCol, 1);
    EXPECT_EQ(range.endCol, 1);
    EXPECT_EQ(range.startLine, 1);
    EXPECT_EQ(range.endLine, 1);

    auto range2 = Range{ 3, 5 };
    auto range3 = range + range2;
    EXPECT_EQ(range3.startCol, 1);
    EXPECT_EQ(range3.endCol, 5);
    EXPECT_EQ(range3.startLine, 1);
    EXPECT_EQ(range3.endLine, 3);

    range += range2;
    EXPECT_EQ(range.startCol, 1);
    EXPECT_EQ(range.endCol, 5);
    EXPECT_EQ(range.startLine, 1);
    EXPECT_EQ(range.endLine, 3);
}

TEST(ZeeBasic_Compiler_Range, Increment)
{
    auto range = Range{ 1, 1 };
    EXPECT_EQ(range.startCol, 1);
    EXPECT_EQ(range.endCol, 1);
    EXPECT_EQ(range.startLine, 1);
    EXPECT_EQ(range.endLine, 1);

    ++range;
    EXPECT_EQ(range.startCol, 1);
    EXPECT_EQ(range.endCol, 2);
    EXPECT_EQ(range.startLine, 1);
    EXPECT_EQ(range.endLine, 1);   

    ++range;
    EXPECT_EQ(range.startCol, 1);
    EXPECT_EQ(range.endCol, 3);
    EXPECT_EQ(range.startLine, 1);
    EXPECT_EQ(range.endLine, 1);
}