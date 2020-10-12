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

#include "ZeeBasic/Compiler/ConstString.hpp"

using namespace ZeeBasic::Compiler;

TEST(ZeeBasic_Compiler_ConstString, Initialization)
{
    auto emptyStr = ConstString{};
    EXPECT_STREQ(emptyStr.getText(), "");
    EXPECT_EQ(emptyStr.getLength(), 0);

    auto badStr = ConstString{"test", -2};
    EXPECT_STREQ(badStr.getText(), "");
    EXPECT_EQ(badStr.getLength(), 0);

    auto str = ConstString("Hello, World", 5);
    EXPECT_STREQ(str.getText(), "Hello");
    EXPECT_EQ(str.getLength(), 5);
}

TEST(ZeeBasic_Compiler_ConstString, Copying)
{
    auto str = ConstString{"Test", 4};
    auto str2 = str;
    EXPECT_EQ(str.getText(), str2.getText());
    EXPECT_EQ(str.getLength(), str2.getLength());

    auto str3 = ConstString{"World", 5};
    EXPECT_STREQ(str3.getText(), "World");

    str3 = str;
    EXPECT_STREQ(str3.getText(), "Test");
    EXPECT_EQ(str3.getText(), str.getText());
    EXPECT_EQ(str3.getLength(), str.getLength());
}

TEST(ZeeBasic_Compiler_ConstString, Moving)
{
    auto str = ConstString{"Test", 4};
    auto ptr = str.getText();
    auto len = str.getLength();

    auto str2 = std::move(str);
    EXPECT_EQ(ptr, str2.getText());
    EXPECT_EQ(len, str2.getLength());

    auto str3 = ConstString{"World", 5};
    EXPECT_STREQ(str3.getText(), "World");

    str3 = std::move(str2);
    EXPECT_STREQ(str3.getText(), "Test");
    EXPECT_EQ(str3.getText(), ptr);
    EXPECT_EQ(str3.getLength(), len);
}

TEST(ZeeBasic_Compiler_ConstString, Comparison)
{
    auto str = ConstString("Hello", 5);
    EXPECT_EQ(str == "Hello", true);
    EXPECT_EQ(str == "hello", true);
    EXPECT_EQ(str == "HELLO", true);
    EXPECT_EQ(str == "Helo", false);    
}
