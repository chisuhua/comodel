#include "inc/ThreadItem.h"
#include "inc/Warp.h"
#include "inc/Instruction.h"
#include "inc/Message.h"

using namespace model;

ThreadItem::ThreadItem(ThreadBlock* block,
		uint32_t threadIdX,
		uint32_t threadIdY,
		uint32_t threadIdZ,
		uint32_t threadId,
		uint32_t warpId,
		uint32_t laneId)
{
	m_block = block;
	// Set absolute ids
	m_threadIdX = threadIdX;
	m_threadIdY = threadIdY;
	m_threadIdZ = threadIdZ;
	m_threadId = threadId;
	m_warpId = warpId;
	m_laneId = laneId;
    m_PC = 0;

    // m_kernel_addr = (uint64_t)disp_info->kernel_addr;
    // m_kernel_args = (uint64_t)disp_info->kernel_args;

	// Set the status of the thread item to be active
	status = ThreadItemStatusActive;

    // InstFuncTable.resize(Instruction::FormatCount);

}


void ThreadItem::Execute(shared_ptr<Instruction> inst)
{
  address_type pc = next_instr();

  set_npc(pc + inst->GetSize());

  try {
    clearRPC();

    if (is_done()) {
      printf( "attempted to execute instruction on a thread that is already " "done.\n");
      assert(0);
    }
    if (inst->is_warp_op() & !this->is_leading_thread()) {
    } else {
        inst->Execute(this);
    }
    // Run exit instruction if exit option included
    // if (inst->is_exit()) exit_impl(inst, this);

    update_pc();
  } catch (int x) {
    // printf("GPGPU-Sim PTX: ERROR (%d) executing intruction \n", x);
    // printf("GPGPU-Sim PTX:       '%s'\n", pI->get_source());
    abort();
  }

}

uint32_t ThreadItem::ReadSReg(int sreg) {
	return m_warp->getSregUint(sreg);
}


void ThreadItem::WriteSReg(int sreg, uint32_t value) {
	// Set scalar register and update VCCZ and EXECZ if necessary.
	m_warp->setSregUint(sreg, value);
}

uint32_t ThreadItem::ReadVReg(int vreg)
{
	assert(vreg >= 0);
	assert(vreg < 256);

	return this->vreg[vreg].as_uint;
}

void ThreadItem::WriteVReg(int vreg, uint32_t value)
{
	assert(vreg >= 0);
	assert(vreg < 256);
	this->vreg[vreg].as_uint = value;
}

uint32_t ThreadItem::ReadReg(int reg)
{
	if (reg < 256) {
		return ReadSReg(reg);
	} else {
		return ReadVReg(reg - 256);
	}
}

int ThreadItem::ReadBitmaskSReg(int sreg)
{
	uint32_t mask = 1;
	if (m_laneId < 32) {
		mask <<= m_laneId;
		return (ReadSReg(sreg) & mask) >> m_laneId;
	} else {
		mask <<= (m_laneId - 32);
		return (ReadSReg(sreg + 1) & mask) >> (m_laneId - 32);
	}
}

void ThreadItem::WriteBitmaskSReg(int sreg, uint32_t value)
{
	uint32_t mask = 1;
	uint32_t bitfield;
	Register new_field;
	if (m_laneId < 32) {
		mask <<= m_laneId;
		bitfield = ReadSReg(sreg);
		new_field.as_uint = (value) ? bitfield | mask: bitfield & ~mask;
		WriteSReg(sreg, new_field.as_uint);
	} else {
		mask <<= (m_laneId - 32);
		bitfield = ReadSReg(sreg + 1);
		new_field.as_uint = (value) ? bitfield | mask: bitfield & ~mask;
		WriteSReg(sreg + 1, new_field.as_uint);
	}
}

// Initialize a buffer resource descriptor
void ThreadItem::ReadBufferResource(
	int sreg,
	BufferDescriptor &buf_desc)
{
	// Buffer resource descriptor is stored in 4 succesive scalar registers
	((uint32_t *) &buf_desc)[0] = m_warp->getSregUint(sreg);
	((uint32_t *) &buf_desc)[1] = m_warp->getSregUint(sreg + 1);
	((uint32_t *) &buf_desc)[2] = m_warp->getSregUint(sreg + 2);
	((uint32_t *) &buf_desc)[3] = m_warp->getSregUint(sreg + 3);
}

// Initialize a mempry pointer descriptor
void ThreadItem::ReadMemPtr(
	int sreg,
	MemoryPointer &mem_ptr)
{
	// Memory pointer descriptor is stored in 2 succesive scalar registers
	((unsigned *)&mem_ptr)[0] = m_warp->getSregUint(sreg);
	((unsigned *)&mem_ptr)[1] = m_warp->getSregUint(sreg + 1);
}

// Initialize a mempry pointer descriptor
void ThreadItem::ReadVRegMemPtr(
	int vreg,
	MemoryPointer &mem_ptr)
{
	// Memory pointer descriptor is stored in 2 succesive scalar registers
	((unsigned *)&mem_ptr)[0] = ReadVReg(vreg);
	((unsigned *)&mem_ptr)[1] = ReadVReg(vreg + 1);
}

void ThreadItem::ReadMemory(uint32_t addr, uint32_t size, char* data)
{
	// Memory pointer descriptor is stored in 2 succesive scalar registers
    m_block->ReadMemory(m_warpId, addr, data, size);

}

void ThreadItem::WriteMemory(uint32_t addr, uint32_t size, char* data)
{
	// Memory pointer descriptor is stored in 2 succesive scalar registers
    m_block->WriteMemory(m_warpId, addr, data, size);
}

void ThreadItem::ReadLDS(uint32_t addr, uint32_t size, char* data)
{
    assert(0);
}

void ThreadItem::WriteLDS(uint32_t addr, uint32_t size, char* data)
{
    assert(0);
}

