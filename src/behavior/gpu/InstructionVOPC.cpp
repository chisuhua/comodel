#include "inc/Instruction.h"
#include "inc/InstructionCommon.h"

#define opcode bytes.VOPC

void InstructionVOPC::Decode(uint64_t _opcode) {
    bytes.dword = _opcode;
    info.op = opcode.op;
	if (bytes.VOPC.src0 == 0xFF) {
		m_size = 8;
	} else {
        bytes.word[1] = 0;
		m_size = 4;
    }
}

// vcc = (S0.f < S1.f).
void Instruction::V_CMP_LT_F32(ThreadItem *item)
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

	// Compare the operands.
	result.as_uint = (s0.as_float < s1.as_float);

	// Write the results.
	WriteBitmaskSReg(RegisterVcc, result.as_uint);

}

// vcc = (S0.f > S1.f).
void Instruction::V_CMP_GT_F32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register result;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 255)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Compare the operands.
	result.as_uint = (s0.as_float > s1.as_float);

	// Write the results.
	WriteBitmaskSReg(RegisterVcc, result.as_uint);

}

// vcc = (S0.f >= S1.f).
void Instruction::V_CMP_GE_F32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register result;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 255)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Compare the operands.
	result.as_uint = (s0.as_float >= s1.as_float);

	// Write the results.
	WriteBitmaskSReg(RegisterVcc, result.as_uint);

}

// vcc = !(S0.f > S1.f).
void Instruction::V_CMP_NGT_F32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register result;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 255)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Compare the operands.
	result.as_uint = !(s0.as_float > s1.as_float);

	// Write the results.
	WriteBitmaskSReg(RegisterVcc, result.as_uint);

}

// vcc = !(S0.f == S1.f).
void Instruction::V_CMP_NEQ_F32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register result;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 255)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Compare the operands.
	result.as_uint = !(s0.as_float == s1.as_float);

	// Write the results.
	WriteBitmaskSReg(RegisterVcc, result.as_uint);

}

#if 0
// vcc = (S0.d < S1.d).
void Instruction::V_CMP_LT_F64(ThreadItem *item)
{
	ISAUnimplemented(item);
}

// vcc = (S0.d == S1.d).
void Instruction::V_CMP_EQ_F64(ThreadItem *item)
{
	ISAUnimplemented(item);
}

// vcc = (S0.d <= S1.d).
void Instruction::V_CMP_LE_F64(ThreadItem *item)
{
	ISAUnimplemented(item);
}

// vcc = (S0.d > S1.d).
void Instruction::V_CMP_GT_F64(ThreadItem *item)
{
	ISAUnimplemented(item);
}

// vcc = !(S0.d >= S1.d).
void Instruction::V_CMP_NGE_F64(ThreadItem *item)
{
	ISAUnimplemented(item);
}

// vcc = !(S0.d == S1.d).
void Instruction::V_CMP_NEQ_F64(ThreadItem *item)
{
	ISAUnimplemented(item);
}

// vcc = !(S0.d < S1.d). 
void Instruction::V_CMP_NLT_F64(ThreadItem *item)
{
	ISAUnimplemented(item);
}
#endif


// vcc = (S0.i < S1.i).
void Instruction::V_CMP_LT_I32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register result;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 255)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Compare the operands.
	result.as_uint = (s0.as_int < s1.as_int);

	// Write the results.
	WriteBitmaskSReg(RegisterVcc, result.as_uint);

}

// vcc = (S0.i == S1.i).
void Instruction::V_CMP_EQ_I32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register result;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 255)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Compare the operands.
	result.as_uint = (s0.as_int == s1.as_int);

	// Write the results.
	WriteBitmaskSReg(RegisterVcc, result.as_uint);

}

// vcc = (S0.i <= S1.i).
void Instruction::V_CMP_LE_I32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register result;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 255)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Compare the operands.
	result.as_uint = (s0.as_int <= s1.as_int);

	// Write the results.
	WriteBitmaskSReg(RegisterVcc, result.as_uint);

}

// vcc = (S0.i > S1.i).
void Instruction::V_CMP_GT_I32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register result;

	// Load operands from registers or as a literal constant.
	if (opcode.src0 == 255)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Compare the operands.
	result.as_uint = (s0.as_int > s1.as_int);

	// Write the results.
	WriteBitmaskSReg(RegisterVcc, result.as_uint);

}

// vcc = (S0.i <> S1.i).
void Instruction::V_CMP_NE_I32(ThreadItem *item)
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

	// Compare the operands.
	result.as_uint = (s0.as_int != s1.as_int);

	// Write the results.
	WriteBitmaskSReg(RegisterVcc, result.as_uint);

}

// D.u = (S0.i >= S1.i).
void Instruction::V_CMP_GE_I32(ThreadItem *item)
{
	Register s0;
	Register s1;
	Register result;

	// Load operands from registers.
	if (opcode.src0 == 0xFF)
		s0.as_uint = opcode.lit_const;
	else
		s0.as_uint = ReadReg(opcode.src0);
	s1.as_uint = ReadVReg(opcode.vsrc1);

	// Compare the operands.
	result.as_uint = (s0.as_int >= s1.as_int);

	// Write the results.
	WriteBitmaskSReg(RegisterVcc, result.as_uint);

}

// D = IEEE numeric class function specified in S1.u, performed on S0.d.
/*
void Instruction::V_CMP_CLASS_F64(ThreadItem *item)
{
	ISAUnimplemented(item);
}
*/

// vcc = (S0.u < S1.u).
void Instruction::V_CMP_LT_U32(ThreadItem *item)
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

	// Compare the operands.
	result.as_uint = (s0.as_uint < s1.as_uint);

	// Write the results.
	WriteBitmaskSReg(RegisterVcc, result.as_uint);

}

// vcc = (S0.u == S1.u).
void Instruction::V_CMP_EQ_U32(ThreadItem *item)
{
	ISAUnimplemented(item);
}

// vcc = (S0.u <= S1.u).
void Instruction::V_CMP_LE_U32(ThreadItem *item)
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

	// Compare the operands.
	result.as_uint = (s0.as_uint <= s1.as_uint);

	// Write the results.
	WriteBitmaskSReg(RegisterVcc, result.as_uint);

}

// vcc = (S0.u > S1.u).
void Instruction::V_CMP_GT_U32(ThreadItem *item)
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

	// Compare the operands.
	result.as_uint = (s0.as_uint > s1.as_uint);

	// Write the results.
	WriteBitmaskSReg(RegisterVcc, result.as_uint);

}

// D.u = (S0.f != S1.f).
void Instruction::V_CMP_NE_U32(ThreadItem *item)
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

	// Compare the operands.
	result.as_uint = (s0.as_uint != s1.as_uint);

	// Write the results.
	WriteBitmaskSReg(RegisterVcc, result.as_uint);

}


void Instruction::V_CMP_GE_U32(ThreadItem *item)
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

	// Compare the operands.
	result.as_uint = (s0.as_uint >= s1.as_uint);

	// Write the results.
	WriteBitmaskSReg(RegisterVcc, result.as_uint);

}


