#pragma once
#include "inc/Module.h"
#include "inc/MsgQueue.h"
#include "inc/MemoryImpl.h"
#include "inc/MemReg.h"
#include "inc/Mmu.h"


#include <vector>

namespace model {


class MemReadPacket : public Packet {
    public:
    MemReadPacket() {}

    PacketType GetType() {
        return PacketType::MemRead;
    }


    uint64_t    addr;
    void*       data;
    uint32_t    length;
    bool        is_pa;
};

class MemWritePacket : public Packet {
    public:
    MemWritePacket() {}

    PacketType GetType() {
        return PacketType::MemWrite;
    }

    uint64_t    addr;
    void*       data;
    uint32_t    length;
    bool        is_pa;
};

class AddrTranslatePacket : public Packet {
    public:
    AddrTranslatePacket() {}
    PacketType GetType() {
        return PacketType::AddrTranslate;
    }

    uint64_t    in_addr;
    void*       out_addr;
    bool        get_emu_addr;  // workaournd to access emulated memory's address to back-door access directory
};

class MemAccessMessage : public Message {
    public:
        MemAccessMessage(shared_ptr<Packet> pkt, shared_ptr<Message> ref_msg = nullptr)
            : Message(MessageType::MEM_REQ, pkt, ref_msg)
        {};

        ~MemAccessMessage() {};

        shared_ptr<MemReadPacket> GetMemReadPacket() {
            return static_pointer_cast<MemReadPacket>(this->m_pkt);
        }

        shared_ptr<MemWritePacket> GetMemWritePacket() {
            return static_pointer_cast<MemWritePacket>(this->m_pkt);
        }

        shared_ptr<AddrTranslatePacket> GetTranslatePacket() {
            return static_pointer_cast<AddrTranslatePacket>(this->m_pkt);
        }
};


class Memory : public Module {
    public:
        Memory(Engine* engine, const std::string &instance_name = "", uint32_t fb_size = 1024*1024)
            : Module(engine, "mc", instance_name)
            // , m_memory("mem", 64*1024)
            , m_fb_size(fb_size)
        {
        };
        virtual ~Memory() {};

        // virtual void FetchInput();

        virtual bool Execute();


    private:
        void HandleMemRead(shared_ptr<MemReadPacket> pkt);
        void HandleMemWrite(shared_ptr<MemWritePacket> pkt);
        void HandleTranslate(shared_ptr<AddrTranslatePacket> pkt);
        void ProcessRegAccess(MsgPtr msg);
        void ProcessMemAccess(MsgPtr msg);
        void ProcessDmaCopy(MsgPtr msg) ;
        bool IsDevAddr(addr_t va) ;
        addr_t GetDevPA(addr_t va) ;
        // bool Translate(addr_t va, addr_t &pa) {
        PTE TranslateVA(addr_t va, addr_t &pa) ;
//        addr_t Allocate(addr_t va) ;
//        void IsAllocate(addr_t va) ;
/*
        void ReadMemory(const uint64_t addr, uint64_t length, void* data){
            m_global_memory.read(addr, length, data);
        }

        void WriteMemory(const uint64_t addr, uint64_t length, void* data){
            m_global_memory.write(addr, length, data);
        }
        */

    private:
        // MemoryImpl<4096>        m_memory;

        uint64_t                m_pt_base;
        uint32_t                m_pd0_addr_bit_num;
        uint32_t                m_pd1_addr_bit_num;
        uint32_t                m_pde_addr_bit_num;
        uint32_t                m_pte_addr_bit_num;

        addr_t GetPteEntryPtrFromPde(addr_t vfn);
        addr_t GetPdeEntryPtrFromPd1(addr_t vfn);
        addr_t GetPd1EntryPtrFromPd0(addr_t vfn);
        addr_t GetPd0EntryPtrFromRoot(addr_t vfn);

        std::map<uint64_t, PTE>  m_tlb;

        uint32_t                m_fb_size;
#define REG_ADDR(regname)
#define REG(regname)                            \
        reg_##regname           m_##regname;
#include "inc/MemReg.inc"

#define REG(regname)
#define REG_ADDR(regname)                       \
        reg_##regname           m_##regname;
#include "inc/MemReg.inc"
};
}
