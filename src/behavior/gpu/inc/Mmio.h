#pragma once
#include "inc/Module.h"
#include "inc/MsgQueue.h"

namespace model {

class Mmio : public Module {
    public:
        Mmio(Engine* engine, const std::string &instance_name = "")
            : Module(engine, "mmio", instance_name)
        {
        };
        virtual ~Mmio() {};

        // virtual void FetchInput();

        virtual bool Execute();

        void Reset();
        bool Busy();

        // TODO: to add reg addres to module name mapping
        void WriteRegister(uint32_t offset, uint32_t value);
        void ReadRegister(uint32_t offset, uint32_t* value);

        void Write(uint32_t offset, void* data, uint32_t length, uint32_t bar);
        void Read(uint32_t offset, void* data, uint32_t length, uint32_t bar);

        int m_bar_base[4] {0};
};
}
