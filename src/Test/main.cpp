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

#include <iostream>

#include "ZeeBasic/Compiler/Analyzer.hpp"
#include "ZeeBasic/Compiler/CTranslator.hpp"
#include "ZeeBasic/Compiler/Error.hpp"
#include "ZeeBasic/Compiler/FileSourceReader.hpp"
#include "ZeeBasic/Compiler/Parser.hpp"
#include "ZeeBasic/Compiler/Program.hpp"

using namespace ZeeBasic::Compiler;

int main(int argc, char* argv[])
{
	auto source = FileSourceReader{ "test.zb" };

	try
	{
		auto program = Program{};
		auto parser = Parser{ source, program };
		parser.run();

		auto analyzer = Analyzer{ program };
		analyzer.run();

		auto translator = CTranslator{ "out.c", program };
		translator.run();
	}
	catch (const Error& err)
	{
		std::cerr << "Compile Error!" << std::endl;
		std::cerr << err.what() << std::endl;
		return -1;
	}

	return 0;
}
