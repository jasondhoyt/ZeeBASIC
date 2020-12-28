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
#include <string>
#include <vector>

#include "ITranslator.hpp"

#include "Symbol.hpp"
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

		void translate(const Nodes::AssignmentStatementNode& node) override;
		void translate(const Nodes::BinaryExpressionNode& node) override;
		void translate(const Nodes::BooleanLiteralNode& node) override;
		void translate(const Nodes::CastExpressionNode& node) override;
		void translate(const Nodes::FunctionCallExpressionNode& node) override;
		void translate(const Nodes::IdentifierExpressionNode& node) override;
		void translate(const Nodes::IntegerLiteralNode& node) override;
		void translate(const Nodes::PrintStatementNode& node) override;
		void translate(const Nodes::RealLiteralNode& node) override;
		void translate(const Nodes::StringLiteralNode& node) override;

	private:
		FILE* m_file = nullptr;

		const Program& m_program;

		enum class IndexType
		{
			Temporary,
			Local
		};

		struct VariableIndex
		{
			IndexType indexType;
			Type type;
			union
			{
				int id;
				const Symbol* symbol = nullptr;
			};

			VariableIndex(const Symbol& symbol) : indexType(IndexType::Local), type(symbol.type), symbol(&symbol) { }
			VariableIndex(Type type) : indexType(IndexType::Temporary), type(type), id(0) { }
		};
		std::vector<VariableIndex> m_variableIndices;

		int m_nextTempId = 1;

		class Writer
		{
		public:
			Writer(std::vector<VariableIndex>& variableIndices);
			void setFile(FILE* file);

			void indent();
			void pushIndent();
			void popIndent();

			Writer& operator<<(char ch);
			Writer& operator<<(const char* text);
			Writer& operator<<(int64_t value);
			Writer& operator<<(bool value);
			Writer& operator<<(int index);
			Writer& operator<<(const VariableIndex& index);
			Writer& operator<<(const Symbol& symbol);

		private:
			std::vector<VariableIndex>& m_variableIndices;
			FILE* m_outFile = nullptr;
			int m_indent = 0;
		};

		Writer m_writer;

		int pushIndex(const Symbol& symbol);
		int pushIndex(const Type& type);
		VariableIndex popIndex();
		void destroyIndex(const VariableIndex& index);
	};

}
