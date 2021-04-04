#include "inc/Memory.h"
#include "inc/Message.h"
#include "inc/RegDef.h"
#include "inc/CopyEngine.h"
#include "inc/Mmu.h"
#include "inc/chunk_generator.h"
#include "inc/Engine.h"
#include "inc/MemoryImpl.h"
#include "common/utils.h"

using namespace model;

bool Memory::Execute() {
    bool busy = Module::Execute();

    for (auto msg = m_msg_active_list.begin(); msg != m_msg_active_list.end(); ) {
        switch ((*msg)->GetMessageType()) {
            case MessageType::CE_REQ: {
                ProcessDmaCopy(*msg);
                msg = m_msg_active_list.erase(msg);
                break;
            }
            case MessageType::MEM_REQ: {
                ProcessMemAccess(*msg);
                PushResp(*msg, make_shared<RespDoneMessage>(*msg));
                msg = m_msg_active_list.erase(msg);
                break;
            }
            default: {
                msg++;
                break;
            }
        }
        busy = true;
    }
    return busy;
}

void Memory::ProcessMemAccess(MsgPtr msg) {
    auto mem_msg = static_pointer_cast<MemAccessMessage>(msg);
    PacketType type = mem_msg->GetType();
    switch (type) {
        case PacketType::MemRead: {
            HandleMemRead(mem_msg->GetMemReadPacket());
            break;
        }
        case PacketType::MemWrite: {
            HandleMemWrite(mem_msg->GetMemWritePacket());
            break;
        }
        case PacketType::AddrTranslate: {
            HandleTranslate(mem_msg->GetTranslatePacket());
            break;
        }
        default:
            debug_print("Memory Process unknow Msg Type");
    }
}

void Memory::ProcessRegAccess(MsgPtr msg){
    auto reg_access_msg = static_pointer_cast<RegAccessMessage>(msg);
    shared_ptr<RegAccessPacket> pkt = reg_access_msg->GetRegAccessPacket();
    uint32_t offset = pkt->offset;
    if (pkt->type == RegOpType::ReadReg) {
        if (offset < MMU_GLB_REG_SIZE) {
            offset = offset >> 2;
            switch (offset) {
#define REG(regname) \
                case regname : {         \
                    *(pkt->read_value) = m_##regname.val; \
                    break;              \
                }
#define REG_ADDR(regname)                                   \
                case regname##_HI : {                       \
                    *(pkt->read_value) = m_##regname.bits.hi; \
                    break;                                  \
                }                                           \
                case regname##_LO : {                       \
                    *(pkt->read_value) = m_##regname.bits.lo; \
                    break;                                  \
                }
#include "inc/MemReg.inc"
                default: {
                    assert("offset don't match any cp base reg");
                    break;
                }
            }
        }
    } else if (pkt->type == RegOpType::WriteReg) {
        if (offset < MMU_GLB_REG_SIZE) {
            offset = offset >> 2;
            switch (offset) {
#define REG(regname)                                        \
                case regname : {                            \
                    debug_print("MODEL_DEBUG: WriteReg %s(%x) with value %x\n", #regname, pkt->offset, pkt->write_value); \
                    m_##regname.val = pkt->write_value;     \
                    break;                                  \
                }
#define REG_ADDR(regname)                                   \
                case regname##_HI : {                       \
                    debug_print("MODEL_DEBUG: WriteRegHi %s(%x) with value %x\n", #regname,  pkt->offset, pkt->write_value); \
                    m_##regname.bits.hi = pkt->write_value; \
                    break;                                  \
                }                                           \
                case regname##_LO : {                       \
                    debug_print("MODEL_DEBUG: WriteRegLo %s(%x) with value %x\n", #regname,  pkt->offset, pkt->write_value); \
                    m_##regname.bits.lo = pkt->write_value; \
                    break;                                  \
                }
#include "inc/MemReg.inc"
                default: {
                    assert("offset don't match any cp base reg");
                    break;
                }
            }
            /*
            if ((offset == MMU_PT_BASE_HI) || (offset == MMU_PT_BASE_LO)) {
                m_pt_base = PackHighLow(m_MMU_PT_BASE_HI.val, m_MMU_PT_BASE_LO.val);
            }
            */
            if (offset == MMU_PT_CFG) {
                m_pd0_addr_bit_num = m_MMU_PT_CFG.bits.pd0_bit;
                m_pd1_addr_bit_num = m_MMU_PT_CFG.bits.pd1_bit;
                m_pde_addr_bit_num = m_MMU_PT_CFG.bits.pde_bit;
                m_pte_addr_bit_num = m_MMU_PT_CFG.bits.pte_bit;
            }
        }
    }

    PushResp(msg, make_shared<RespDoneMessage>(msg));

}

// TODO MMU
void Memory::HandleMemRead(shared_ptr<MemReadPacket> pkt){
    uint64_t addr = pkt->addr;
    if (!pkt->is_pa) {
        addr = GetDevPA(pkt->addr);
    }
    m_engine->m_global_mem->read(addr, pkt->length, pkt->data);
    debug_print("MemRead: va %lx, pa %lx, size %d, data %x\n", pkt->addr, addr, pkt->length, *(uint32_t*)(pkt->data));
}

void Memory::HandleMemWrite(shared_ptr<MemWritePacket> pkt){
    uint64_t addr = pkt->addr;
    if (!pkt->is_pa) {
        addr = GetDevPA(pkt->addr);
    }
    m_engine->m_global_mem->write(addr, pkt->length, pkt->data);
    debug_print("MemWrite: va %lx, pa %lx, size %d, data %x\n", pkt->addr, addr, pkt->length, *(uint32_t*)(pkt->data));
}

void Memory::HandleTranslate(shared_ptr<AddrTranslatePacket> pkt){
    uint64_t in_addr = pkt->in_addr;
    addr_t *out_addr = (addr_t*)pkt->out_addr;
    *out_addr = GetDevPA(in_addr);
    if (pkt->get_emu_addr) {
        uint64_t data;
        m_engine->m_global_mem->get_addr(*out_addr, &data);
        *out_addr = data;
    }
}

void Memory::ProcessDmaCopy(MsgPtr msg) {
    auto ce_msg = static_pointer_cast<CeReqMessage>(msg);
    shared_ptr<DmaCopyPacket> pkt = ce_msg->GetDmaCopyPacket();
    addr_t src =  addr_t(pkt->src);
    addr_t dst =  addr_t(pkt->dst);


    bool is_src_dev = IsDevAddr(src);
    bool is_dst_dev = IsDevAddr(dst);

    debug_print("DmaCopy Info: src %lx, dst %lx, src_dev? %s, dst_dev? %s", src, dst,  is_src_dev? "yes":"no" , is_dst_dev? "yes":"no" );

    void* tmp = malloc(CACHELINE_SIZE*sizeof(uint8_t));

    for (ChunkGenerator gen(src, pkt->length, CACHELINE_SIZE); !gen.done(); gen.next()) {
        uint32_t cur_size = gen.size();
        uint32_t offset = gen.complete();
        addr_t cur_src = src + offset;
        addr_t cur_dst = dst + offset;

        if (is_src_dev) {
            addr_t pa = GetDevPA(cur_src);
            debug_print("src_dev read %x, bytes from VA: %lx, %lx\n", cur_size, cur_src, pa);
            m_engine->m_global_mem->read(pa, cur_size, tmp);
        } else {
            debug_print("src_sys read %x, bytes from %lx\n", cur_size, cur_src);
            memcpy(tmp, (void*)cur_src, cur_size);
        }

        if (is_dst_dev) {
            addr_t pa = GetDevPA(cur_dst);
            debug_print("dst_dev write %x bytes to VA:%lx, PA:%lx dump write data:\n", cur_size, cur_dst, pa);
            for (uint32_t i = 0; i < cur_size; i +=4) {
                debug_print(" %x", *((uint32_t*)(tmp) + i/4));
            }
            debug_print("\n");
            m_engine->m_global_mem->write(pa, cur_size, tmp);
        } else {
            debug_print("dst_sys write %x, bytes to %lx", cur_size, cur_dst);
            memcpy((void*)cur_dst, tmp, cur_size);
        }
    }

    // auto pkt = make_shared<DmaCopyPacket>();
    PushResp(msg, make_shared<RespDoneMessage>(msg));
}

bool Memory::IsDevAddr(addr_t va) {
    /*
    if (m_MMU_PT_CFG.bits.physical_mode == 1) {
        if ((va >= m_MMU_DEV_PA_RANGE_BASE.val) && (va <= m_MMU_DEV_PA_RANGE_TOP.val))
            return true;
    } else {
    */
    // not matter is physical_mode, non-canicol address is dev
    if (va >= (1ULL<< 48)) return true;

    addr_t pa;
    PTE pte = TranslateVA(va, pa);
    return ((pte.info.valid == 1) && (pte.info.system == 0x0));
}

// user should make sure is dev addr
addr_t Memory::GetDevPA(addr_t va) {
    if (m_MMU_PT_CFG.bits.physical_mode == 1 && (va > (1ULL << 48))) {
        // return (va - m_MMU_DEV_PA_RANGE_BASE.val);
        return (va - (1ULL<< 48));
    } else {
        addr_t pa;
        PTE pte = TranslateVA(va, pa);
        if (!pte.info.valid) {
            printf("ERROR: invalid TLB translation, va %lx, pa %lx\n", va, pa);
            assert(0);
        }
        return pa;
    }
}

// return false which is not in dev range
/*
PageStatus Memory::Translate(addr_t va, addr_t &pa) {
    PageStatus page {0};
        }
    } else {
        return TranslateVA(va, pa);
    }
    addr_t pa;
    return TranslateVA(va, pa);
}
*/

PTE Memory::TranslateVA(addr_t va, addr_t &pa) {

    addr_t  vfn = ((va & 0xFFFFFFFFFFFFFC00LL) >> PAGE_4KB_BIT_NUM);

    PTE pte = {0};
    auto it = m_tlb.find(vfn);

    if (it != m_tlb.end()) {
        pte = m_tlb[vfn];
    } else {
        addr_t pte_ptr = GetPteEntryPtrFromPde(vfn);

        if (pte_ptr == INVALID_ADDR) return pte;

        m_engine->m_global_mem->read(pte_ptr, sizeof(uint64_t), &pte.data);

        // pf checkeck, now only valid check
        if (pte.info.valid == 0) {
            return pte;
        }
        m_tlb.insert(make_pair(vfn, pte));
    }

    addr_t trans_pa = pte.info.address << PAGE_4KB_BIT_NUM;
    pa = trans_pa + (va & ((1 << PAGE_4KB_BIT_NUM) - 1));

    return pte;
}

addr_t Memory::GetPteEntryPtrFromPde(addr_t vfn) {
    addr_t pa = 0;

    pa = GetPdeEntryPtrFromPd1(vfn);
    if (pa == INVALID_ADDR) return INVALID_ADDR;

    PDE pde;
    m_engine->m_global_mem->read(pa, sizeof(pde.data), &pde.data);

    if (pde.info.valid == 0) return INVALID_ADDR;

    pa = (pde.info.address << PAGE_4KB_BIT_NUM) +
        (vfn & ((1 << m_pte_addr_bit_num) - 1))*sizeof(PTE);

    return pa;
}

addr_t Memory::GetPdeEntryPtrFromPd1(addr_t vfn) {
    addr_t pa = 0;

    pa = GetPd1EntryPtrFromPd0(vfn);
    if (pa == INVALID_ADDR) return INVALID_ADDR;

    PDE pde;
    m_engine->m_global_mem->read(pa, sizeof(pde.data), &pde.data);
    if (pde.info.valid == 0) return INVALID_ADDR;

    pa = (pde.info.address << PAGE_4KB_BIT_NUM) +
        ((vfn >> m_pte_addr_bit_num) & ((1 << m_pde_addr_bit_num) - 1))*sizeof(PDE);

    return pa;
}

addr_t Memory::GetPd1EntryPtrFromPd0(addr_t vfn) {
    addr_t pa = 0;

    pa = GetPd0EntryPtrFromRoot(vfn);
    if (pa == INVALID_ADDR) return INVALID_ADDR;

    PDE pde;
    m_engine->m_global_mem->read(pa, sizeof(pde.data), &pde.data);
    if (pde.info.valid == 0) return INVALID_ADDR;
    pa = (pde.info.address << PAGE_4KB_BIT_NUM) +
        ((vfn >> (m_pte_addr_bit_num + m_pde_addr_bit_num)) & ((1 << m_pd1_addr_bit_num) - 1))*sizeof(PDE);

    return pa;
}

addr_t Memory::GetPd0EntryPtrFromRoot(addr_t vfn) {
    addr_t pa = 0;
    bool is_pd0_disable = (m_pd0_addr_bit_num == 0);
    bool is_pd1_disable = (m_pd1_addr_bit_num == 0);
    bool is_pde_disable = (m_pde_addr_bit_num == 0);

    if (!is_pd0_disable) {
        pa = m_MMU_PT_BASE.val + ((vfn >> (37 - m_pd0_addr_bit_num)) &
                            ((1 << m_pd0_addr_bit_num) - 1));
    } else if (!is_pd1_disable) {
        pa = m_MMU_PT_BASE.val + ((vfn >> (37 - m_pd0_addr_bit_num - m_pd1_addr_bit_num)) &
                            ((1 << m_pd1_addr_bit_num) - 1));
    } else if (!is_pde_disable) {
        pa = m_MMU_PT_BASE.val + ((vfn >> (37 - m_pd0_addr_bit_num - m_pd1_addr_bit_num - m_pde_addr_bit_num)) &
                            ((1 << m_pde_addr_bit_num) - 1));
    } else {
        assert("GetRootPa failed");
    }

    return pa;
}


