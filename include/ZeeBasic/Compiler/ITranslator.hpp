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

namespace ZeeBasic::Compiler
{

	namespace Nodes
	{
		class AssignmentStatementNode;
		class BinaryExpressionNode;
		class BooleanLiteralNode;
		class CastExpressionNode;
		class FunctionCallExpressionNode;
		class IdentifierExpressionNode;
		class IntegerLiteralNode;
		class PrintStatementNode;
		class RealLiteralNode;
		class StringLiteralNode;
	}

	struct Program;

	class ITranslator
	{
	public:
		ITranslator() { }
		virtual ~ITranslator() { }

		virtual void run() = 0;

		virtual void translate(const Nodes::AssignmentStatementNode& node) = 0;
		virtual void translate(const Nodes::BinaryExpressionNode& node) = 0;
		virtual void translate(const Nodes::BooleanLiteralNode& node) = 0;
		virtual void translate(const Nodes::CastExpressionNode& node) = 0;
		virtual void translate(const Nodes::FunctionCallExpressionNode& node) = 0;
		virtual void translate(const Nodes::IdentifierExpressionNode& node) = 0;
		virtual void translate(const Nodes::IntegerLiteralNode& node) = 0;
		virtual void translate(const Nodes::PrintStatementNode& node) = 0;
		virtual void translate(const Nodes::RealLiteralNode& node) = 0;
		virtual void translate(const Nodes::StringLiteralNode& node) = 0;
	};

}
