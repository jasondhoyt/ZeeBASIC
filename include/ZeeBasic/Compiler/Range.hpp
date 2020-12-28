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

#include <cstdint>

namespace ZeeBasic::Compiler
{

    // Defines a pair of line/column locations that designate a piece of source code.
    struct Range
    {
        Range() : startCol(0), endCol(0), startLine(0), endLine(0) { }
        Range(int lineNo, int colNo) : startCol(colNo), endCol(colNo), startLine(lineNo), endLine(lineNo) { }
        Range(const Range& start, const Range& end)
            :
            startCol(start.startCol),
            endCol(end.endCol),
            startLine(start.startLine),
            endLine(end.endLine)
        { }
        ~Range() { }

        Range(const Range&) = default;
        Range(Range&&) = default;
        Range& operator=(const Range&) = default;
        Range& operator=(Range&&) = default;

        uint64_t startCol : 8;
        uint64_t endCol : 8;
        uint64_t startLine : 24;
        uint64_t endLine : 24;

        Range& operator+=(const Range& rhs)
        {
            endCol = rhs.endCol;
            endLine = rhs.endLine;
            return *this;
        }

        Range& operator++()
        {
            ++endCol;
            return *this;
        }

        friend Range operator+(Range lhs, const Range& rhs)
        {
            lhs += rhs;
            return lhs;
        }

        bool operator==(const Range& rhs) const
        {
            return startCol == rhs.startCol && endCol == rhs.endCol && startLine == rhs.startLine && endLine == rhs.endLine;
        }
    };

}
