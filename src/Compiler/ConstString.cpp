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

#include <memory>
#include <vector>

#include "ZeeBasic/Compiler/ConstString.hpp"

namespace ZeeBasic::Compiler
{

    constexpr auto kCharBufferSize = 2048;

    struct CharBuffer
    {
        int used;
        char buf[kCharBufferSize];
    };

    static thread_local std::vector<std::unique_ptr<CharBuffer>> buffers;
    static constexpr auto kEmptyString = "";

    static const char* allocConstString(const char* text, int len)
    {
        if (len <= 0)
        {
            return kEmptyString;
        }

        if (buffers.empty() || buffers.back()->used + len + 1 > kCharBufferSize)
        {
            buffers.emplace_back(std::make_unique<CharBuffer>());
        }

        auto ptr = buffers.back()->buf + buffers.back()->used;
        buffers.back()->used += len + 1;

        memcpy(ptr, text, len);
        ptr[len] = 0;

        return ptr;
    }

    ConstString::ConstString()
        :
        m_text(kEmptyString),
        m_length(0)
    { }

    ConstString::ConstString(const char* text, int len)
        :
        m_text(allocConstString(text, len)),
        m_length(len > 0 ? len : 0)
    { }

    ConstString::~ConstString()
    { }

    bool ConstString::operator==(const char* text) const
    {
#ifdef _WIN32
        return _stricmp(m_text, text) == 0;
#else
        return strcasecmp(m_text, text) == 0;
#endif
    }

    bool ConstString::operator==(const ConstString& str) const
    {
        if (m_length == str.m_length)
        {
#ifdef _WIN32
            return _stricmp(m_text, str.m_text) == 0;
#else
            return strcasecmp(m_text, str.m_text) == 0;
#endif
        }

        return false;
    }

    bool ConstString::endsWith(char ch) const
    {
        if (m_length > 0)
        {
            return m_text[m_length - 1] == ch;
        }

        return false;
    }

}
