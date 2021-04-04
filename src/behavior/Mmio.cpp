#include "inc/Mmio.h"
#include "inc/Message.h"
#include "inc/RegDef.h"
#include "inc/Memory.h"

using namespace model;

bool Mmio::Execute() {
    bool busy = Module::Execute();
    return busy;
};

void Mmio::Read(uint32_t offset, void* data, uint32_t length, uint32_t bar) {

    printf("MODEL_DEBUG(MMIO): %s read to address %x\n", __FUNCTION__, offset);
    std::string block = "mc";
    auto pkt = make_shared<MemReadPacket>();
    pkt->addr = m_bar_base[bar] + offset;
    pkt->is_pa = true;
    pkt->data = data;
    pkt->length = length;

    auto msg = make_shared<MemAccessMessage>(pkt);
    PushReq("mc", msg);

    m_response_queue->Push(msg);
    while(!msg->IsRespDone()) {};
    printf("MODEL_DEBUG(MMIO): %s read done %x\n", __FUNCTION__, offset);
};

void Mmio::Write(uint32_t offset, void* data, uint32_t length, uint32_t bar) {

    printf("MODEL_DEBUG(MMIO): %s write to address %x\n", __FUNCTION__, offset);
    std::string block = "mc";
    auto pkt = make_shared<MemWritePacket>();
    pkt->addr = m_bar_base[bar] + offset;
    pkt->is_pa = true;
    pkt->data = data;
    pkt->length = length;

    auto msg = make_shared<MemAccessMessage>(pkt);
    PushReq("mc", msg);

    m_response_queue->Push(msg);
    while(!msg->IsRespDone()) {};
    printf("MODEL_DEBUG(MMIO): %s write done %x\n", __FUNCTION__, offset);
};


// TODO: to add reg addres to module name mapping
void Mmio::WriteRegister(uint32_t offset, uint32_t value) {

    printf("MODEL_DEBUG(MMIO): %s write to address %x\n", __FUNCTION__, offset);
    std::string block = DecodeRegSpace(offset);
    auto pkt = make_shared<RegAccessPacket>();
    pkt->type = RegOpType::WriteReg;
    pkt->offset = offset;
    pkt->write_value = value;

    auto msg = make_shared<RegAccessMessage>(pkt);
    PushReq(block, msg);

    m_response_queue->Push(msg);
    printf("MODEL_DEBUG(MMIO): %s write done %x\n", __FUNCTION__, offset);
};

void Mmio::ReadRegister(uint32_t offset, uint32_t* value) {
    printf("MODEL_DEBUG(MMIO): %s read to address %x\n", __FUNCTION__, offset);

    std::string block = DecodeRegSpace(offset);
    auto pkt = make_shared<RegAccessPacket>();
    pkt->type = RegOpType::ReadReg;
    pkt->offset = offset;
    pkt->read_value = value;

    auto msg = make_shared<RegAccessMessage>(pkt);
    PushReq(block, msg);

    m_response_queue->Push(msg);

    while(!msg->IsRespDone()) {};
    printf("MODEL_DEBUG(MMIO): %s read done %x\n", __FUNCTION__, offset);
}

