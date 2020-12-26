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

#include "ExpressionNode.hpp"

namespace ZeeBasic::Compiler::Nodes
{

	class BinaryExpressionNode
		:
		public ExpressionNode
	{
	public:
		enum class Operator
		{
			Add,
			Subtract,
			Multiply,
			Divide,
			IntDivide
		};

		BinaryExpressionNode(Operator op, std::unique_ptr<ExpressionNode> lhs, std::unique_ptr<ExpressionNode> rhs);
		virtual ~BinaryExpressionNode();

		auto getOperator() const { return m_op; }
		auto& getLeft() const { return *m_lhs; }
		auto& getRight() const { return *m_rhs; }

		void parse(IParser& parser) override;
		void analyze(IAnalyzer& analyzer) override;
		void translate(ITranslator& translator) override;

	private:
		Operator m_op;
		std::unique_ptr<ExpressionNode> m_lhs;
		std::unique_ptr<ExpressionNode> m_rhs;		
	};

}
