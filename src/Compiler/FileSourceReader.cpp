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

#include <cstdio>
#include <stdexcept>

#include "ZeeBasic/Compiler/FileSourceReader.hpp"

namespace ZeeBasic::Compiler
{

    FileSourceReader::FileSourceReader(const std::string& path)
        :
        m_size(0),
        m_data(),
        m_offset(0),
        m_lineNo(1),
        m_colNo(1)
    {
#ifdef _WIN32
        FILE* file = nullptr;
        fopen_s(&file, path.c_str(), "r");
#else
        auto file = fopen(path.c_str(), "r");
#endif
        if (!file)
        {
            throw std::runtime_error(std::string{"Failed to open source file : "} + path);
        }

        fseek(file, 0, SEEK_END);
        auto len = size_t(ftell(file));
        fseek(file, 0, SEEK_SET);

        auto data = std::make_unique<char[]>(len);
        auto readBytes = fread(data.get(), sizeof(char), len, file);
        fclose(file);
        if (readBytes != len)
        {
            throw std::runtime_error(std::string{"Failed to read source file : "} + path);
        }

        m_data = std::move(data);
        m_size = len;
    }

    FileSourceReader::~FileSourceReader()
    { }

    char FileSourceReader::readNextChar()
    {
        if (m_offset >= m_size)
        {
            return 0;
        }

        auto ch = m_data[m_offset++];

        if (ch == '\n')
        {
            ++m_lineNo;
            m_colNo = 1;
        }
        else
        {
            ++m_colNo;
        }

        return ch;
    }

}
