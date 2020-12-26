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

#include <cstdio>
#include <stack>
#include <string>

#include "ITranslator.hpp"

#include "Type.hpp"

namespace ZeeBasic::Compiler
{

	class CTranslator
		:
		public ITranslator
	{
	public:
		CTranslator(const std::string& path, const Program& program);
		virtual ~CTranslator();

		void run() override;

		void translate(const Nodes::BinaryExpressionNode& node) override;
		void translate(const Nodes::IntegerLiteralNode& node) override;
		void translate(const Nodes::PrintStatementNode& node) override;

	private:
		FILE* m_file;

		const Program& m_program;

		int m_indent;

		struct Temporary
		{
			Type type;
			int id;

			Temporary(Type type, int id) : type(type), id(id) { }
		};
		std::stack<Temporary> m_temps;

		int m_nextTempId;

		void indent();
		int makeTemp(Type type);
		void destroyTemp(const Temporary& temp);
	};

}
