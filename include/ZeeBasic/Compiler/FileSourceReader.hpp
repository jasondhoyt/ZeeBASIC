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

namespace ZeeBasic::Compiler
{

    class FileSourceReader
        :
        public ISourceReader
    {
    public:
        FileSourceReader(const std::string& path);
        ~FileSourceReader();

        FileSourceReader(FileSourceReader&) = delete;
        FileSourceReader(FileSourceReader&&) = default;
        FileSourceReader& operator=(FileSourceReader&) = delete;
        FileSourceReader& operator=(FileSourceReader&&) = default;

        // Get the current read position within the stream.
        void getReadPosition(int& lineNo, int& colNo) override { lineNo = m_lineNo; colNo = m_colNo; }

        // Read the next character in the source stream, or a 0 if the stream is complete.
        char readNextChar() override;

    private:
        // data read from file
        size_t m_size;
        std::unique_ptr<char[]> m_data;

        // current read position
        size_t m_offset;
        int m_lineNo;
        int m_colNo;
    };

}
