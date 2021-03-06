#include "inc/Instruction.h"
#include "inc/InstructionCommon.h"
#include "inc/OperandUtil.h"
#include <cmath>
#include <limits>

#define opcode bytes.VOP1

// TODO auto generate it from coasm
void InstructionVOP1::Decode(uint64_t _opcode) {
    bytes.dword = _opcode;
    info.op = opcode.op;
	if (opcode.src0 == 0xFF) {
		m_size = 8;
	} else {
        bytes.word[1] = 0;
		m_size = 4;
    }
    // FIXME on E32 format 
}

// Do nothing
void Instruction::V_NOP(ThreadItem *item)
{
	// Do nothing
}

// D.u = S0.u.
void Instruction::V_MOV_B32(ThreadItem *item)
{
	Register value;

	// Load operand from register or as a literal constant.
	if (opcode.src0 == 0xFF)
		value.as_uint = opcode.lit_const;
	else {
        if (opcode.ssrc0_ == 0)
		    value.as_uint = ReadSReg(opcode.src0);
        else
		    value.as_uint = ReadVReg(opcode.src0);
    }

	// Write the results.
	WriteVReg(opcode.vdst, value.as_uint);

}

// Copy one VGPR value to one SGPR.
void Instruction::V_READFIRSTLANE_B32(ThreadItem *item)
{
	Register value;

	// Load operand from register.
	assert(opcode.ssrc0_ == 1 || opcode.src0 == RegisterM0);
    if (opcode.ssrc0_ == 1) {
	    value.as_uint = ReadVReg(opcode.src0);
    } else {
	    value.as_uint = ReadSReg(opcode.src0);
    }

	// Write the results.
	// Store the data in the destination register
	WriteSReg(opcode.vdst, value.as_uint);

}

// D.i = (int)S0.d.
void Instruction::V_CVT_I32_F64(ThreadItem *item)
{
	ISAUnimplemented(item);
}

// D.f = (double)S0.i.
void Instruction::V_CVT_F64_I32(ThreadItem *item)
{
	union
	{
		double as_double;
		unsigned as_reg[2];

	} value;
	Register s0;
	Register result_lo;
	Register result_hi;

	// Load operand from register or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else {
	    s0.as_uint = ReadReg(opcode.src0 + opcode.ssrc0_ * 0x100);
    }


	// Convert and separate value.
	value.as_double = (double) s0.as_int;

	// Write the results.
	result_lo.as_uint = value.as_reg[0];
	result_hi.as_uint = value.as_reg[1];
	WriteVReg(opcode.vdst, result_lo.as_uint);
	WriteVReg(opcode.vdst + 1, result_hi.as_uint);

}

// D.f = (float)S0.i.
void Instruction::V_CVT_F32_I32(ThreadItem *item)
{
	Register s0;
	Register value;

	// Load operand from register or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else {
        if (opcode.ssrc0_ == 0)
		    s0.as_uint = ReadSReg(opcode.src0);
        else
		    s0.as_uint = ReadVReg(opcode.src0);
    }

	value.as_float = (float) s0.as_int;

	// Write the results.
	WriteVReg(opcode.vdst, value.as_uint);

}

// D.f = (float)S0.u.
void Instruction::V_CVT_F32_U32(ThreadItem *item)
{
	Register s0 ;
	Register value;

	// Load operand from register or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else {
        if (opcode.ssrc0_ == 0)
		    s0.as_uint = ReadSReg(opcode.src0);
        else
		    s0.as_uint = ReadVReg(opcode.src0);
    }


	value.as_float = (float) s0.as_uint;

	// Write the results.
	WriteVReg(opcode.vdst, value.as_uint);

}

// D.i = (uint)S0.f.
void Instruction::V_CVT_U32_F32(ThreadItem *item)
{
	Register s0;
	Register value;

	float fvalue;

	// Load operand from register or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else {
        if (opcode.ssrc0_ == 0)
		    s0.as_uint = ReadSReg(opcode.src0);
        else
		    s0.as_uint = ReadVReg(opcode.src0);
    }

	fvalue = s0.as_float;

	// Handle special number cases and cast to an unsigned

	// -inf, NaN, 0, -0 --> 0
	if ((std::isinf(fvalue) && fvalue < 0.0f) || std::isnan(fvalue)
		|| fvalue == 0.0f || fvalue == -0.0f)
		value.as_uint = 0;
	// inf, > max_uint --> max_uint
	else if (std::isinf(fvalue) || fvalue >= std::numeric_limits<unsigned int>::max())
		value.as_uint = std::numeric_limits<unsigned int>::max();
	else
		value.as_uint = (unsigned) fvalue;

	// Write the results.
	WriteVReg(opcode.vdst, value.as_uint);

}

// D.i = (int)S0.f.
void Instruction::V_CVT_I32_F32(ThreadItem *item)
{
	Register s0;
	Register value;

	float fvalue;

	// Load operand from register or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else {
        if (opcode.ssrc0_ == 0)
		    s0.as_uint = ReadSReg(opcode.src0);
        else
		    s0.as_uint = ReadVReg(opcode.src0);
    }


	fvalue = s0.as_float;

	// Handle special number cases and cast to an int

	// inf, > max_int --> max_int
	if ((std::isinf(fvalue) && fvalue > 0.0f) || fvalue >= std::numeric_limits<int>::max())
		value.as_int = std::numeric_limits<int>::max();
	// -inf, < -max_int --> -max_int
	else if (std::isinf(fvalue) || fvalue < std::numeric_limits<int>::min())
		value.as_int = std::numeric_limits<int>::min();
	// NaN, 0, -0 --> 0
	else if (std::isnan(fvalue) || fvalue == 0.0f || fvalue == -0.0f)
		value.as_int = 0;
	else
		value.as_int = (int) fvalue;

	// Write the results.
	WriteVReg(opcode.vdst, value.as_uint);

}

// D.f = (float)S0.d.
void Instruction::V_CVT_F32_F64(ThreadItem *item)
{
	union
	{
		double as_double;
		unsigned as_reg[2];

	} s0;
	Register value;

	assert(opcode.src0 != 0xFF);

	// Load operand from registers.
	s0.as_reg[0] = ReadReg(opcode.src0 + opcode.ssrc0_ * 0x100);
	s0.as_reg[1] = ReadReg(opcode.src0 + opcode.ssrc0_ * 0x100 + 1);

	// Cast to a single precision float
	value.as_float = (float) s0.as_double;

	// Write the results.
	WriteVReg(opcode.vdst, value.as_uint);

}


// D.d = (double)S0.f.
void Instruction::V_CVT_F64_F32(ThreadItem *item)
{
	Register s0;
	union
	{
		double as_double;
		unsigned as_reg[2];

	} value;
	Register value_lo;
	Register value_hi;

	// Load operand from register or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0 + opcode.ssrc0_ * 0x100);

	// Cast to a single precision float
	value.as_double = (double) s0.as_float;

	// Write the results.
	value_lo.as_uint = value.as_reg[0];
	value_hi.as_uint = value.as_reg[1];
	WriteVReg(opcode.vdst, value_lo.as_uint);
	WriteVReg(opcode.vdst + 1, value_hi.as_uint);

}

// D.d = (double)S0.u.
void Instruction::V_CVT_F64_U32(ThreadItem *item)
{
	ISAUnimplemented(item);
}

// D.f = trunc(S0.f), return integer part of S0.
void Instruction::V_TRUNC_F32(ThreadItem *item)
{
	Register s0;
	Register value;

	// Load operand from register or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0 + opcode.ssrc0_ * 0x100);

	// Truncate decimal portion
	value.as_float = (float)((int)s0.as_float);

	// Write the results.
	WriteVReg(opcode.vdst, value.as_uint);

}

// D.f = trunc(S0); if ((S0 < 0.0) && (S0 != D)) D += -1.0.
void Instruction::V_FLOOR_F32(ThreadItem *item)
{
	ISAUnimplemented(item);
}

// D.f = log2(S0.f).
void Instruction::V_LOG_F32(ThreadItem *item)
{
	ISAUnimplemented(item);
}

// D.f = 1.0 / S0.f.
void Instruction::V_RCP_F32(ThreadItem *item)
{
	Register s0;
	Register rcp;

	// Load operand from register or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0 + opcode.ssrc0_ * 0x100);

	rcp.as_float = 1.0f / s0.as_float;

	// Write the results.
	WriteVReg(opcode.vdst, rcp.as_uint);

}

// D.f = 1.0 / sqrt(S0.f).
void Instruction::V_RSQ_F32(ThreadItem *item)
{
	Register s0;
	Register rsq;

	// Load operand from register or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0 + opcode.ssrc0_ * 0x100);

	rsq.as_float = 1.0f / sqrt(s0.as_float);

	// Write the results.
	WriteVReg(opcode.vdst, rsq.as_uint);

}


// D.d = 1.0 / (S0.d).
void Instruction::V_RCP_F64(ThreadItem *item)
{
	ISAUnimplemented(item);
}

// D.f = 1.0 / sqrt(S0.f).
void Instruction::V_RSQ_F64(ThreadItem *item)
{
	ISAUnimplemented(item);
}

// D.f = sqrt(S0.f).
void Instruction::V_SQRT_F32(ThreadItem *item)
{
	Register s0;
	Register srt;

	// Load operand from register or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0 + opcode.ssrc0_ * 0x100);

	srt.as_float = sqrtf(s0.as_float);

	// Write the results.
	WriteVReg(opcode.vdst, srt.as_uint);

}

// D.f = sin(S0.f)
void Instruction::V_SIN_F32(ThreadItem *item)
{
	Register s0;
	Register result;

	// Load operand from register or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0 + opcode.ssrc0_ * 0x100);

	// Normalize input
	// XXX Should it be module instead of dividing?
	s0.as_float = s0.as_float * (2 * M_PI);

	if (inRange(s0.as_float, -256, 256))
	{
		result.as_float = sinf(s0.as_float);
	}
	else
	{
		assert(0); // Haven't debugged this yet
		result.as_float = 0;
	}

	// Write the results.
	WriteVReg(opcode.vdst, result.as_uint);

}

// D.f = cos(S0.f)
void Instruction::V_COS_F32(ThreadItem *item)
{
	Register s0;
	Register result;

	// Load operand from register or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0 + opcode.ssrc0_ * 0x100);

	// Normalize input
	// XXX Should it be module instead of dividing?
	s0.as_float = s0.as_float * (2 * M_PI);

	if (inRange(s0.as_float, -256, 256))
	{
		result.as_float = cosf(s0.as_float);
	}
	else
	{
		assert(0); // Haven't debugged this yet
		result.as_float = 1;
	}

	// Write the results.
	WriteVReg(opcode.vdst, result.as_uint);

}

// D.u = ~S0.u.
void Instruction::V_NOT_B32(ThreadItem *item)
{
	Register s0;
	Register result;

	// Load operand from register or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0 + opcode.ssrc0_ * 0x100);

	// Bitwise not
	result.as_uint = ~s0.as_uint;

	// Write the results.
	WriteVReg(opcode.vdst, result.as_uint);

}

// D.u = position of first 1 in S0 from MSB; D=0xFFFFFFFF if S0==0.
void Instruction::V_FFBH_U32(ThreadItem *item)
{
	ISAUnimplemented(item);
}

// D.d = FRAC64(S0.d);
void Instruction::V_FRACT_F64(ThreadItem *item)
{
	ISAUnimplemented(item);
}

// VGPR[D.u + M0.u] = VGPR[S0.u].
void Instruction::V_MOVRELD_B32(ThreadItem *item)
{
	Register s0;
	Register m0;

	assert(opcode.src0 != 0xFF);

	// Load operand from register or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0 + opcode.ssrc0_ * 0x100);
	m0.as_uint = ReadReg(RegisterM0);

	// Write the results.
	WriteVReg(opcode.vdst+m0.as_uint, s0.as_uint);

}


// VGPR[D.u] = VGPR[S0.u + M0.u].
void Instruction::V_MOVRELS_B32(ThreadItem *item)
{
	Register s0;
	Register m0;

	assert(opcode.src0 != 0xFF);

	// Load operand from register or as a literal constant.
	m0.as_uint = ReadReg(RegisterM0);
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0 + opcode.ssrc0_ * 0x100 + m0.as_uint);

	// Write the results.
	WriteVReg(opcode.vdst, s0.as_uint);

}

// D.i = (double)S0.i.
void Instruction::V_SEXT_I64_I32(ThreadItem *item)
{
    Int s0;

	// Load operand from register or as a literal constant.
	if (opcode.src0 == 0xFF)
		s0.u32 = opcode.lit_const;
	else {
	    s0.u32 = ReadReg(opcode.src0 + opcode.ssrc0_ * 0x100);
    }

    Long value;
    value.i64 = SEXT_I64_I32(s0.i32);

	// Write the results.
	WriteVReg(opcode.vdst, value.int32[0].u32);
	WriteVReg(opcode.vdst + 1, value.int32[1].u32);

}

