#include "inc/Instruction.h"
#include "inc/InstructionCommon.h"

#define opcode bytes.VOP2

void InstructionVOP2::Decode(uint64_t _opcode) {
    bytes.dword = _opcode;
    info.op = opcode.op;
	/* 0xFF indicates the use of a literal constant as a
	 * source operand. */
	if (opcode.src0 == 0xFF) {
		m_size = 8;
	} else if (opcode.op == 32) {
	 /* the instruction */
		m_size = 8;
	} else {
        bytes.word[1] = 0;
		m_size = 4;
    }
}

/* D.u = VCC[i] ? S1.u : S0.u (i = threadID in wave); VOP3: specify VCC as a
 * scalar GPR in S2. */
void Instruction::V_CNDMASK_B32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register result;

	int vcci;

	// Load operands from register or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);
	vcci = ReadBitmaskSReg(RegisterVcc);

	// Calculate the result.
	result.as_uint = (vcci) ? s1.as_uint : s0.as_uint;

	// Write the results.
	WriteVReg(opcode.vdst, result.as_uint);

}

// D.f = S0.f + S1.f.
void Instruction::V_ADD_F32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register sum;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Calculate the sum.
	sum.as_float = s0.as_float + s1.as_float;

	// Write the results.
	WriteVReg(opcode.vdst, sum.as_uint);

}

// D.f = S0.f - S1.f.
void Instruction::V_SUB_F32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register dif;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Calculate the difference.
	dif.as_float = s0.as_float - s1.as_float;

	// Write the results.
	WriteVReg(opcode.vdst, dif.as_uint);

}

// D.f = S1.f - S0.f.
void Instruction::V_SUBREV_F32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register dif;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Calculate the difference.
	dif.as_float = s1.as_float - s0.as_float;

	// Write the results.
	WriteVReg(opcode.vdst, dif.as_uint);

}

// D.f = S0.F * S1.f + D.f.
// void Instruction::V_MAC_LEGACY_F32(ThreadItem *item)


// D.f = S0.f * S1.f (DX9 rules, 0.0*x = 0.0).
/*
void Instruction::V_MUL_LEGACY_F32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register product;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Calculate the product.
	if (s0.as_float == 0.0f || s1.as_float == 0.0f)
	{
		product.as_float = 0.0f;
	}
	else
	{
		product.as_float = s0.as_float * s1.as_float;
	}

	// Write the results.
	WriteVReg(opcode.vdst, product.as_uint);

}
*/

// D.f = S0.f * S1.f.
void Instruction::V_MUL_F32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register product;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Calculate the product.
	product.as_float = s0.as_float * s1.as_float;

	// Write the results.
	WriteVReg(opcode.vdst, product.as_uint);

}

// D.i = S0.i[23:0] * S1.i[23:0].
void Instruction::V_MUL_I32_I24(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register product;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Truncate operands to 24-bit signed integers
	s0.as_uint = SignExtend(s0.as_uint, 24);
	s1.as_uint = SignExtend(s1.as_uint, 24);

	// Calculate the product.
	product.as_int = s0.as_int * s1.as_int;

	// Write the results.
	WriteVReg(opcode.vdst, product.as_uint);

}

// D.f = min(S0.f, S1.f).
void Instruction::V_MIN_F32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register min;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Calculate the minimum operand.
	if (s0.as_float < s1.as_float)
	{
		min.as_float = s0.as_float;
	}
	else
	{
		min.as_float = s1.as_float;
	}

	// Write the results.
	WriteVReg(opcode.vdst, min.as_uint);

}

// D.f = max(S0.f, S1.f).
void Instruction::V_MAX_F32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register max;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Calculate the minimum operand.
	if (s0.as_float > s1.as_float)
	{
		max.as_float = s0.as_float;
	}
	else
	{
		max.as_float = s1.as_float;
	}

	// Write the results.
	WriteVReg(opcode.vdst, max.as_uint);

}

// D.i = max(S0.i, S1.i).
void Instruction::V_MAX_I32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register max;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Calculate the minimum operand.
	if (s0.as_int > s1.as_int)
	{
		max.as_int = s0.as_int;
	}
	else
	{
		max.as_int = s1.as_int;
	}

	// Write the results.
	WriteVReg(opcode.vdst, max.as_uint);

}

// D.i = min(S0.i, S1.i).
void Instruction::V_MIN_I32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register min;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Calculate the minimum operand.
	if (s0.as_int < s1.as_int)
	{
		min.as_int = s0.as_int;
	}
	else
	{
		min.as_int = s1.as_int;
	}

	// Write the results.
	WriteVReg(opcode.vdst, min.as_uint);

}

// D.u = min(S0.u, S1.u).
void Instruction::V_MIN_U32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register min;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Calculate the minimum operand.
	if (s0.as_uint < s1.as_uint)
	{
		min.as_uint = s0.as_uint;
	}
	else
	{
		min.as_uint = s1.as_uint;
	}

	// Write the results.
	WriteVReg(opcode.vdst, min.as_uint);

}

// D.u = max(S0.u, S1.u).
void Instruction::V_MAX_U32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register max;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Calculate the maximum operand.
	if (s0.as_uint > s1.as_uint)
	{
		max.as_uint = s0.as_uint;
	}
	else
	{
		max.as_uint = s1.as_uint;
	}

	// Write the results.
	WriteVReg(opcode.vdst, max.as_uint);

}

// D.u = S1.u >> S0.u[4:0].
void Instruction::V_LSHRREV_B32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register result;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
	{
		assert(opcode.lit_const < 32);
		s0.as_uint = opcode.lit_const;
	}
	else
	{
		s0.as_uint = ReadReg(opcode.src0) & 0x1F;
	}
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Right shift s1 by s0.
	result.as_uint = s1.as_uint >> s0.as_uint;

	// Write the results.
	WriteVReg(opcode.vdst, result.as_uint);

}

// D.i = S1.i >> S0.i[4:0].
void Instruction::V_ASHRREV_I32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register result;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
	{
		assert(opcode.lit_const < 32);
		s0.as_uint = opcode.lit_const & 0x1F;
	}
	else
	{
		s0.as_uint = ReadReg(opcode.src0) & 0x1F;
	}
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Right shift s1 by s0.
	result.as_int = s1.as_int >> s0.as_int;

	// Write the results.
	WriteVReg(opcode.vdst, result.as_uint);

}

// D.u = S0.u << S1.u[4:0].
void Instruction::V_LSHL_B32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register result;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1) & 0x1F;

	// Left shift s1 by s0.
	result.as_uint = s0.as_uint << s1.as_uint;

	// Write the results.
	WriteVReg(opcode.vdst, result.as_uint);

}

// D.u = S1.u << S0.u[4:0].
void Instruction::V_LSHLREV_B32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register result;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
	{
		assert(opcode.lit_const < 32);
		s0.as_uint = opcode.lit_const;
	}
	else
	{
		s0.as_uint = ReadReg(opcode.src0) & 0x1F;
	}
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Left shift s1 by s0.
	result.as_uint = s1.as_uint << s0.as_uint;

	// Write the results.
	WriteVReg(opcode.vdst, result.as_uint);

}

// D.u = S0.u & S1.u.
void Instruction::V_AND_B32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register result;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
	{
		s0.as_uint = opcode.lit_const;
	}
	else
	{
		s0.as_uint = ReadReg(opcode.src0);
	}
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Bitwise OR the two operands.
	result.as_uint = s0.as_uint & s1.as_uint;

	// Write the results.
	WriteVReg(opcode.vdst, result.as_uint);

}

// D.u = S0.u | S1.u.
void Instruction::V_OR_B32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register result;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Bitwise OR the two operands.
	result.as_uint = s0.as_uint | s1.as_uint;

	// Write the results.
	WriteVReg(opcode.vdst, result.as_uint);

}

// D.u = S0.u ^ S1.u.
void Instruction::V_XOR_B32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register result;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Bitwise OR the two operands.
	result.as_uint = s0.as_uint ^ s1.as_uint;

	// Write the results.
	WriteVReg(opcode.vdst, result.as_uint);

}

//D.u = ((1<<S0.u[4:0])-1) << S1.u[4:0]; S0=bitfield_width, S1=bitfield_offset.
void Instruction::V_BFM_B32(ThreadItem *item)
{
	ISAUnimplemented(item);
}

// D.f = S0.f * S1.f + D.f.
void Instruction::V_MAC_F32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register dst;
	Register result;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);
	dst.as_uint = ReadVReg(opcode.vdst);

	// Calculate the result.
	result.as_float = s0.as_float * s1.as_float + dst.as_float;

	// Write the results.
	WriteVReg(opcode.vdst, result.as_uint);

}

// D.f = S0.f * K + S1.f; K is a 32-bit inline constant
void Instruction::V_MADMK_F32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register K;
	Register dst;

	// Load operands from registers or as a literal constant.
	s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);
	K.as_uint = opcode.lit_const;

	// Calculate the result
	dst.as_float = s0.as_float * K.as_float + s1.as_float;

	// Write the results.
	WriteVReg(opcode.vdst, dst.as_uint);

}

// D.u = S0.u + S1.u, vcc = carry-out.
void Instruction::V_ADD_I32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register sum;
	Register carry;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Calculate the sum and carry.
	sum.as_int = s0.as_int + s1.as_int;
	carry.as_uint =
		! !(((long long) s0.as_int + (long long) s1.as_int) >> 32);

	// Write the results.
	WriteVReg(opcode.vdst, sum.as_uint);
	WriteBitmaskSReg(RegisterVcc, carry.as_uint);

}

// D.u = S0.u + S1.u, vcc = carry-out.
void Instruction::V_ADD_U32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register sum;
	// Register carry;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Calculate the sum and carry.
	sum.as_uint = s0.as_uint + s1.as_uint;
	// carry.as_uint =
	//	! !(((long long) s0.as_uint + (long long) s1.as_uint) >> 32);

	// Write the results.
	WriteVReg(opcode.vdst, sum.as_uint);
	// WriteBitmaskSReg(RegisterVcc, carry.as_uint);

}

// D.u = S0.u + S1.u, vcc = carry-out.
void Instruction::V_ADDCO_U32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register sum;
	Register carry;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Calculate the sum and carry.
	sum.as_uint = s0.as_uint + s1.as_uint;
	carry.as_uint =
		! !(((long long) s0.as_uint + (long long) s1.as_uint) >> 32);

	// Write the results.
	WriteVReg(opcode.vdst, sum.as_uint);
	WriteBitmaskSReg(RegisterVcc, carry.as_uint);

}

// D.u = S0.u - S1.u; vcc = carry-out.
void Instruction::V_SUB_I32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register dif;
	Register carry;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Calculate the difference and carry.
	dif.as_uint = s0.as_int - s1.as_int;
	carry.as_uint = (s1.as_int > s0.as_int);

	// Write the results.
	WriteVReg(opcode.vdst, dif.as_uint);
	WriteBitmaskSReg(RegisterVcc, carry.as_uint);

}

// D.u = S0.u - S1.u; vcc = carry-out.
void Instruction::V_SUB_U32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register dif;
	Register carry;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Calculate the difference and carry.
	dif.as_uint = s0.as_uint - s1.as_uint;
	carry.as_uint = (s1.as_uint > s0.as_uint);

	// Write the results.
	WriteVReg(opcode.vdst, dif.as_uint);
	WriteBitmaskSReg(RegisterVcc, carry.as_uint);

}

// D.u = S1.u - S0.u; vcc = carry-out.
void Instruction::V_SUBREV_I32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register dif;
	Register carry;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Calculate the difference and carry.
	dif.as_int = s1.as_int - s0.as_int;
	carry.as_uint = (s0.as_int > s1.as_int);

	// Write the results.
	WriteVReg(opcode.vdst, dif.as_uint);
	WriteBitmaskSReg(RegisterVcc, carry.as_uint);

}

void Instruction::V_SUBREV_U32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register dif;
	Register carry;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Calculate the difference and carry.
	dif.as_uint = s1.as_uint - s0.as_uint;
	carry.as_uint = (s0.as_uint > s1.as_uint);

	// Write the results.
	WriteVReg(opcode.vdst, dif.as_uint);
	WriteBitmaskSReg(RegisterVcc, carry.as_uint);

}

// D = {flt32_to_flt16(S1.f),flt32_to_flt16(S0.f)}, with round-toward-zero.
/*
void Instruction::V_CVT_PKRTZ_F16_F32(ThreadItem *item)
{
	union hfpack
	{
		unsigned as_uint32;
		struct
		{
			unsigned short s1f;
			unsigned short s0f;
		} as_f16f16;
	};

	Register s0;
	Register s1;
	unsigned short s0f;
	unsigned short s1f;
	union hfpack float_pack;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Convert to half float
	s0f = Float32to16(s0.as_float);
	s1f = Float32to16(s1.as_float);
	float_pack.as_f16f16.s0f = s0f;
	float_pack.as_f16f16.s1f = s1f;

	// Write the results.
	WriteVReg(opcode.vdst, float_pack.as_uint32);

}
*/

