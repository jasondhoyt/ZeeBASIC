# BSD 2-Clause License
#
# Copyright (c) 2020, Jason Hoyt
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE

ifndef EXTERNAL
$(error EXTERNAL is not defined)
endif

CC=g++
CFLAGS=-Wall --std=c++17 -I$(EXTERNAL)/include -I./include

MKDIR=mkdir -p
RM=rm -rf

ifdef RELEASE
CFLAGS+=-O2
else
CFLAGS+=-g -O0
endif

LDFLAGS=-L$(EXTERNAL)/lib

CFLAGS_TEST=$(CFLAGS)
LDFLAGS_TEST=$(LDFLAGS) -lgtest -lgtest_main

UNIT_TESTS=\
	test/bin/Compiler_RangeTest \
	test/bin/Compiler_ErrorTest \
	test/bin/Compiler_ConstStringTest \
	test/bin/Compiler_FileSourceReaderTest \
	test/bin/Compiler_LexicalAnalyzerTest

all: $(UNIT_TESTS)

clean:
	@echo "Cleaning project ..."
	@$(RM) test/bin

test/bin/Compiler_RangeTest: test/Compiler/RangeTest.cpp include/ZeeBasic/Compiler/Range.hpp | test/bin
	@echo "Building Unit Test ... Compiler / RangeTest"
	@$(CC) $(CFLAGS) -o $@ test/Compiler/RangeTest.cpp $(LDFLAGS_TEST)

test/bin/Compiler_ErrorTest: test/Compiler/ErrorTest.cpp include/ZeeBasic/Compiler/Error.hpp src/Compiler/Error.cpp | test/bin
	@echo "Building Unit Test ... Compiler / ErrorTest"
	@$(CC) $(CFLAGS) -o $@ test/Compiler/ErrorTest.cpp src/Compiler/Error.cpp $(LDFLAGS_TEST)

test/bin/Compiler_ConstStringTest: test/Compiler/ConstStringTest.cpp include/ZeeBasic/Compiler/ConstString.hpp src/Compiler/ConstString.cpp | test/bin
	@echo "Building Unit Test ... Compiler / ConstStringTest"
	@$(CC) $(CFLAGS) -o $@ test/Compiler/ConstStringTest.cpp src/Compiler/ConstString.cpp $(LDFLAGS_TEST)

test/bin/Compiler_FileSourceReaderTest: test/Compiler/FileSourceReaderTest.cpp include/ZeeBasic/Compiler/FileSourceReader.hpp src/Compiler/FileSourceReader.cpp | test/bin
	@echo "Building Unit Test ... Compiler / FileSourceReaderTest"
	@$(CC) $(CFLAGS) -o $@ test/Compiler/FileSourceReaderTest.cpp src/Compiler/FileSourceReader.cpp $(LDFLAGS_TEST)

test/bin/Compiler_LexicalAnalyzerTest: test/Compiler/LexicalAnalyzerTest.cpp include/ZeeBasic/Compiler/LexicalAnalyzer.hpp src/Compiler/LexicalAnalyzer.cpp | test/bin
	@echo "Building Unit Test ... Compiler / LexicalAnalyzerTest"
	@$(CC) $(CFLAGS) -o $@ test/Compiler/LexicalAnalyzerTest.cpp src/Compiler/LexicalAnalyzer.cpp src/Compiler/ConstString.cpp src/Compiler/Error.cpp $(LDFLAGS_TEST)

test/bin:
	@$(MKDIR) test/bin
