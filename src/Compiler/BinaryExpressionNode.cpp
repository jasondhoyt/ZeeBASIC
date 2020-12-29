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

#include "ZeeBasic/Compiler/BinaryExpressionNode.hpp"

#include "ZeeBasic/Compiler/CastExpressionNode.hpp"
#include "ZeeBasic/Compiler/Error.hpp"

namespace ZeeBasic::Compiler::Nodes
{

	BinaryExpressionNode::BinaryExpressionNode(Operator op, std::unique_ptr<ExpressionNode> lhs, std::unique_ptr<ExpressionNode> rhs)
		:
		m_op(op),
		m_lhs(std::move(lhs)),
		m_rhs(std::move(rhs))
	{ }

	BinaryExpressionNode::~BinaryExpressionNode()
	{ }

	void BinaryExpressionNode::parse(IParser& parser)
	{ 
		m_range = Range{ m_lhs->getRange(), m_rhs->getRange() };
		
		const auto& lhsType = m_lhs->getType();
		const auto& rhsType = m_rhs->getType();

		enum class ImplicitCastSide
		{
			None,
			Left,
			Right
		};

		static struct {
			int lhsBaseType;
			int rhsBaseType;
			Operator op;
			ImplicitCastSide castSide;
			int castToType;
			bool allowed;
			const char* errorMessage;
		} operationTable[] = {
			//
			// No Implicit Conversions
			//

			// Boolean|Boolean - bitwise & comparison allowed
			{ BaseType_Boolean, BaseType_Boolean, Operator::BitwiseAnd, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Boolean, BaseType_Boolean, Operator::BitwiseOr, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Boolean, BaseType_Boolean, Operator::BitwiseXor, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Boolean, BaseType_Boolean, Operator::Equals, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Boolean, BaseType_Boolean, Operator::NotEquals, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Boolean, BaseType_Boolean, Operator::Less, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Boolean, BaseType_Boolean, Operator::LessEquals, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Boolean, BaseType_Boolean, Operator::Greater, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Boolean, BaseType_Boolean, Operator::GreaterEquals, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Boolean, BaseType_Boolean, Operator::None, ImplicitCastSide::None, BaseType_Unknown, false, "Operation not allowed on boolean types" },

			// Integer|Integer - all allowed
			{ BaseType_Integer, BaseType_Integer, Operator::None, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },

			// Real|Real - no bitwise allowed
			{ BaseType_Real, BaseType_Real, Operator::Add, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Real, BaseType_Real, Operator::Subtract, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Real, BaseType_Real, Operator::Multiply, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Real, BaseType_Real, Operator::Divide, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Real, BaseType_Real, Operator::IntDivide, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Real, BaseType_Real, Operator::Modulus, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Real, BaseType_Real, Operator::Equals, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Real, BaseType_Real, Operator::NotEquals, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Real, BaseType_Real, Operator::Less, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Real, BaseType_Real, Operator::LessEquals, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Real, BaseType_Real, Operator::Greater, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Real, BaseType_Real, Operator::GreaterEquals, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_Real, BaseType_Real, Operator::None, ImplicitCastSide::None, BaseType_Unknown, false, "Bitwise operation not allowed on real types" },

			// String|String - only add allowed
			{ BaseType_String, BaseType_String, Operator::Add, ImplicitCastSide::None, BaseType_Unknown, true, nullptr },
			{ BaseType_String, BaseType_String, Operator::None, ImplicitCastSide::None, BaseType_Unknown, false, "Operation not allowed on string types" },

			//
			// Implicit Conversions
			//

			// Boolean|Integer - not allowed
			// Integer|Boolean - not allowed
			{ BaseType_Boolean, BaseType_Integer, Operator::None, ImplicitCastSide::None, BaseType_Unknown, false, "Implicit cast between integer and boolean not allowed" },
			{ BaseType_Integer, BaseType_Boolean, Operator::None, ImplicitCastSide::None, BaseType_Unknown, false, "Implicit cast between integer and boolean not allowed" },
			
			// Integer|Real - cast up to real (except for bitwise)
			// Real|Integer - cast up to real (except for bitwise)
			{ BaseType_Integer, BaseType_Real, Operator::BitwiseOr, ImplicitCastSide::None, BaseType_Unknown, false, "Bitwise operation not allowed on real types" },
			{ BaseType_Integer, BaseType_Real, Operator::BitwiseAnd, ImplicitCastSide::None, BaseType_Unknown, false, "Bitwise operation not allowed on real types" },
			{ BaseType_Integer, BaseType_Real, Operator::BitwiseXor, ImplicitCastSide::None, BaseType_Unknown, false, "Bitwise operation not allowed on real types" },
			{ BaseType_Real, BaseType_Integer, Operator::BitwiseOr, ImplicitCastSide::None, BaseType_Unknown, false, "Bitwise operation not allowed on real types" },
			{ BaseType_Real, BaseType_Integer, Operator::BitwiseAnd, ImplicitCastSide::None, BaseType_Unknown, false, "Bitwise operation not allowed on real types" },
			{ BaseType_Real, BaseType_Integer, Operator::BitwiseXor, ImplicitCastSide::None, BaseType_Unknown, false, "Bitwise operation not allowed on real types" },
			{ BaseType_Integer, BaseType_Real, Operator::None, ImplicitCastSide::Left, BaseType_Real, true, nullptr },
			{ BaseType_Real, BaseType_Integer, Operator::None, ImplicitCastSide::Right, BaseType_Real, true, nullptr },

			// Boolean|Real - not allowed
			// Real|Boolean - not allowed
			{ BaseType_Boolean, BaseType_Integer, Operator::None, ImplicitCastSide::None, BaseType_Unknown, false, "Implicit cast between real and boolean not allowed" },
			{ BaseType_Integer, BaseType_Boolean, Operator::None, ImplicitCastSide::None, BaseType_Unknown, false, "Implicit cast between real and boolean not allowed" },

			// String|anything - not allowed
			// anything|String - not allowed
			{ BaseType_String, BaseType_Boolean, Operator::None, ImplicitCastSide::None, BaseType_Unknown, false, "Unable to implicitly cast type to string" },
			{ BaseType_String, BaseType_Integer, Operator::None, ImplicitCastSide::None, BaseType_Unknown, false, "Unable to implicitly cast type to string" },
			{ BaseType_String, BaseType_Real, Operator::None, ImplicitCastSide::None, BaseType_Unknown, false, "Unable to implicitly cast type to string" },
			{ BaseType_Boolean, BaseType_String, Operator::None, ImplicitCastSide::None, BaseType_Unknown, false, "Unable to implicitly cast type to string" },
			{ BaseType_Integer, BaseType_String, Operator::None, ImplicitCastSide::None, BaseType_Unknown, false, "Unable to implicitly cast type to string" },
			{ BaseType_Real, BaseType_String, Operator::None, ImplicitCastSide::None, BaseType_Unknown, false, "Unable to implicitly cast type to string" },
			
			{ BaseType_Unknown, BaseType_Unknown, Operator::None, ImplicitCastSide::None, false, BaseType_Unknown, nullptr }
		};

		for (auto i = 0; operationTable[i].lhsBaseType != BaseType_Unknown; ++i)
		{
			if (lhsType.base == operationTable[i].lhsBaseType && rhsType.base == operationTable[i].rhsBaseType)
			{
				if (operationTable[i].op == m_op || operationTable[i].op == Operator::None)
				{
					if (operationTable[i].allowed)
					{
						// casting to be done?
						if (operationTable[i].castSide == ImplicitCastSide::Left)
						{
							m_lhs = std::make_unique<CastExpressionNode>(operationTable[i].castToType, std::move(m_lhs));
						}
						else if (operationTable[i].castSide == ImplicitCastSide::Right)
						{
							m_rhs = std::make_unique<CastExpressionNode>(operationTable[i].castToType, std::move(m_rhs));
						}

						// take on resulting type
						if (m_op == Operator::Divide)
						{
							m_type = Type{ BaseType_Real };
						}
						else if (m_op == Operator::IntDivide)
						{
							m_type = Type{ BaseType_Integer };
						}
						else if (m_op >= Operator::Equals && m_op <= Operator::GreaterEquals)
						{
							m_type = Type{ BaseType_Boolean };
						}
						else
						{
							m_type = m_lhs->getType();
						}

						break;
					}
					else
					{
						throw Error::create(m_range, operationTable[i].errorMessage);
					}
				}
			}
		}
	}

	void BinaryExpressionNode::translate(ITranslator& translator) const
	{
		translator.translate(*this);
	}

}
