#include "inc/StreamProcessor.h"
#include "inc/Message.h"
#include "inc/RegDef.h"
// #include "inc/common.h"
#include "queue.h"
#include "signal.h"
#include "inc/ComputeUnit.h"
#include "inc/CopyEngine.h"
#include "inc/Memory.h"
#include "stream.h"
#include <unistd.h>

inline uint64_t PackHighLow(uint32_t hi, uint32_t lo) {
  uint64_t ret = hi;
  ret = (ret << 32 ) | lo;
  return ret;
}


using namespace model;

// void StreamProcessor::FetchInput() {
//     Module::FetchInput();
// };

void StreamProcessor::LaunchGrid(kernel_dispatch_packet_t& aql_pkt) {
    uint32_t GridDimX = aql_pkt.grid_size_x;
    uint32_t GridDimY = aql_pkt.grid_size_y;
    uint32_t GridDimZ = aql_pkt.grid_size_z;

    bool need_resp = false;
    if (aql_pkt.completion_signal.handle != 0)
        need_resp = true;

    auto msg_list = make_shared<std::list<shared_ptr<Message>>>();
    for (uint32_t blockIdZ = 0; blockIdZ < GridDimZ; blockIdZ++) {
        for (uint32_t blockIdY = 0; blockIdY < GridDimY; blockIdY++) {
            for (uint32_t blockIdX = 0;  blockIdX < GridDimX; blockIdX++) {
                auto pkt = make_shared<CuDispatchPacket>();
                pkt->disp_info.kernel_addr = aql_pkt.kernel_object;
                pkt->disp_info.kernel_args = aql_pkt.kernarg_address;
                pkt->disp_info.kernel_ctrl.val = aql_pkt.kernel_ctrl;
                pkt->disp_info.kernel_mode.val = aql_pkt.kernel_mode;
                // pkt->disp_info.kernel_resource.val = aql_pkt.kernel_resource;
                pkt->disp_info.gridDimX = aql_pkt.grid_size_x;
                pkt->disp_info.gridDimY = aql_pkt.grid_size_y;
                pkt->disp_info.gridDimZ = aql_pkt.grid_size_z;
                pkt->disp_info.blockDimX = aql_pkt.workgroup_size_x;
                pkt->disp_info.blockDimY = aql_pkt.workgroup_size_y;
                pkt->disp_info.blockDimZ = aql_pkt.workgroup_size_z;
                // pkt->disp_info.main_co = m_main_co;
                // pkt->disp_info.root_function = m_root_function;
                pkt->disp_info.blockIdX = blockIdX;
                pkt->disp_info.blockIdY = blockIdY;
                pkt->disp_info.blockIdZ = blockIdZ;
                auto msg = make_shared<CuReqMessage>(pkt);
                if (need_resp) {
                    m_response_queue->Push(msg);
                    msg_list->push_back(static_pointer_cast<Message>(msg));
                }
                PushReq("cu0", msg);
            }
        }
    }

    if (need_resp)
        m_dispatch_response_list.insert(make_pair(aql_pkt.completion_signal.handle, msg_list));
}

void StreamProcessor::TranslateVA(uint64_t va, uint64_t *pa, bool emu_addr) {
    std::string block = "mc";
    auto pkt = make_shared<AddrTranslatePacket>();
    pkt->in_addr = va;
    pkt->out_addr = pa;
    pkt->get_emu_addr = emu_addr;

    auto msg = make_shared<MemAccessMessage>(pkt);
    PushReq("mc", msg);

    // in_translation = true;

    m_response_queue->Push(msg);
}

void StreamProcessor::ProcessRespMsg(MsgPtr msg)  {
    // in_translation = false;
}

void StreamProcessor::LaunchDMA(dma_copy_packet_t& aql_pkt) {
    auto pkt = make_shared<DmaCopyPacket>();
    pkt->src = aql_pkt.src;
    pkt->dst = aql_pkt.dst;
    pkt->length = aql_pkt.bytes;

    bool need_resp = false;
    if (aql_pkt.completion_signal.handle != 0)
        need_resp = true;

    auto msg_list = make_shared<std::list<shared_ptr<Message>>>();
    auto msg = make_shared<CeReqMessage>(pkt);
    if (need_resp) {
        m_response_queue->Push(static_pointer_cast<Message>(msg));
        msg_list->push_back(static_pointer_cast<Message>(msg));
    }
    PushReq("ce", msg);

    if (need_resp) {
        m_dispatch_response_list.insert(make_pair(aql_pkt.completion_signal.handle, msg_list));
    }

    /*
    // dma_copy_dir dir = dma_pkt.copy_dir;
    // memcpy(dst, src, bytes);
    if (dma_pkt.completion_signal.handle != 0) {
        core::SharedSignal* sig = ::core::SharedSignal::Object(dma_pkt.completion_signal);
        uint64_t signal_value = sig->amd_signal.value - 1;
        sig->amd_signal.value = signal_value;
    }
    */
}

void StreamProcessor::ProcessDispatchDone()  {

    for (auto it = m_dispatch_response_list.begin(); it != m_dispatch_response_list.end();) {
        signal_t completion_signal;
        completion_signal.handle = it->first;
        //hsa_signal_t completion_signal = it->first;
        //uint64_t completion_signal = it->first;
        shared_ptr<std::list<shared_ptr<Message>>> msg_list = it->second;

        bool done = true;
        for (auto &msg_it : *msg_list) {
            if (!msg_it->IsRespDone()) {
                done = false;
                break;
            }
        }
        if (done) {
            core::SharedSignal* sig = ::core::SharedSignal::Object(completion_signal);
            uint64_t signal_value = sig->co_signal_.value - 1;
            sig->co_signal_.value = signal_value;
            it = m_dispatch_response_list.erase(it);

        } else {
            it++;
        }

    }
}

			    // LaunchGrid(root_function_, (CmdDispatchPacket*)&pkt, wave_size_, main_co_);
bool StreamProcessor::Execute() {
    bool busy = Module::Execute();

    bool queue_busy = false;

	for(auto it = m_cmd_queues.begin(); it != m_cmd_queues.end(); it++) {
        // uint32_t queue_id = it->first;
        shared_ptr<CommandQueue> cmd_queue = it->second;

        AqlPacket* buffer = static_cast<AqlPacket*>(cmd_queue->base_address);
        // hsa_signal_t signal = cp_queue.hsa_queue.doorbell_signal;
        uint32_t size = cmd_queue->size;

        cmd_queue->write_dispatch_id = *(uint64_t*)(cmd_queue->write_ptr);

        uint64_t read = cmd_queue->read_dispatch_id;
        while (read != cmd_queue->write_dispatch_id) {
            AqlPacket& pkt = buffer[read % size];
            const uint8_t packet_type = pkt.dispatch.header >> PACKET_HEADER_TYPE;

            if (packet_type == PACKET_TYPE_INVALID) {
                printf("ERROR: receive invalid packet int cp");
                break;
            } else if (packet_type == PACKET_TYPE_KERNEL_DISPATCH) {
                kernel_dispatch_packet_t& dispatch_pkt = pkt.dispatch;
                /*
                if (translated_kernel_addr == 0) {
                    TranslateVA((uint64_t)dispatch_pkt.kernel_object, (uint64_t*)&translated_kernel_addr);
                    break;
                } else if (translated_kernel_args == 0) {
                    TranslateVA((uint64_t)dispatch_pkt.kernarg_address, (uint64_t*)&translated_kernel_args);
                    break;
                } else {
                    dispatch_pkt.kernel_object = translated_kernel_addr;
                    dispatch_pkt.kernarg_address = (void*)translated_kernel_args;
                */
			        LaunchGrid(dispatch_pkt);
                    translated_kernel_addr = 0;
                    translated_kernel_args = 0;
                //}
            } else if (packet_type == PACKET_TYPE_DMA_COPY) {
                dma_copy_packet_t& dma_pkt = pkt.dma_copy;
			    LaunchDMA(dma_pkt);
            } else if (packet_type == PACKET_TYPE_BARRIER_AND ||
                packet_type == PACKET_TYPE_BARRIER_OR) {
                if (pkt.barrier_and.completion_signal.handle != 0) {
                    core::SharedSignal* sig = ::core::SharedSignal::Object(pkt.barrier_and.completion_signal);
                    uint64_t signal_value = sig->co_signal_.value - 1;
                    sig->co_signal_.value = signal_value;
                }

            }
            read++;
        	auto header=pkt.dispatch.header;
	        header &= 0xFF00;
            header |= (PACKET_TYPE_INVALID << PACKET_HEADER_TYPE);
	        *(volatile uint16_t*)&pkt.dispatch.header=header;

	        queue_busy = true;
        }
        if (read != cmd_queue->read_dispatch_id) {
            // update read pointer
            cmd_queue->read_dispatch_id = read;
            // update host read_dispatch_id
            *(uint64_t*)(cmd_queue->read_ptr)  = cmd_queue->read_dispatch_id;
        }
    }

    ProcessDispatchDone();

    busy |= queue_busy;

    m_status.bits.busy = busy;

    if (m_ctrl.bits.enable == 1) {
        return true;
    } else if (m_status.bits.active == 0) {
        sleep(1);
        return true;
    }

    return busy;

    // return m_ctrl.bits.stop ? true : false;
};

void StreamProcessor::ProcessRegAccess(MsgPtr msg) {
    auto reg_access_msg = static_pointer_cast<RegAccessMessage>(msg);
    shared_ptr<RegAccessPacket> pkt = reg_access_msg->GetRegAccessPacket();
    uint32_t offset = pkt->offset;
    if (pkt->type == RegOpType::ReadReg) {
        if (offset < CP_GLB_REG_SIZE) {
            switch (offset) {
                case CP_GLB_CTRL: {
                    *(pkt->read_value) = m_ctrl.val;
                    break;
                }
                case CP_GLB_STATUS: {
                    *(pkt->read_value) = m_status.val;
                    break;
                }
                default: {
                    assert("offset don't match any cp base reg");
                    break;
                }
            }
        } else if (offset < CP_MAX_QUEUE_NUM * CP_QUEUE_REG_SIZE ) {
            offset -= CP_GLB_REG_SIZE *4;
            uint32_t queue_id = offset / CP_QUEUE_REG_SIZE;
            offset %= CP_QUEUE_REG_SIZE;
            offset = offset >> 2;
            switch (offset) {
#define READ_CP_QUEUE_REG(regname) \
            case regname : {         \
                *(pkt->read_value) = queue_regs[queue_id][regname]; \
                break;              \
            }
                READ_CP_QUEUE_REG(CP_QUEUE_CTRL);
                READ_CP_QUEUE_REG(CP_QUEUE_RB_BASE_HI);
                READ_CP_QUEUE_REG(CP_QUEUE_RB_BASE_LO);
                READ_CP_QUEUE_REG(CP_QUEUE_RB_SIZE);
                READ_CP_QUEUE_REG(CP_QUEUE_RB_RPTR_HI);
                READ_CP_QUEUE_REG(CP_QUEUE_RB_RPTR_LO);
                READ_CP_QUEUE_REG(CP_QUEUE_RB_WPTR_HI);
                READ_CP_QUEUE_REG(CP_QUEUE_RB_WPTR_LO);
                READ_CP_QUEUE_REG(CP_QUEUE_DOORBELL_BASE_HI);
                READ_CP_QUEUE_REG(CP_QUEUE_DOORBELL_BASE_LO);
                READ_CP_QUEUE_REG(CP_QUEUE_DOORBELL_OFFSET);
                default: {
                    assert("offset don't match any queue base reg");
                    break;
                }
            }
        }
    } else if (pkt->type == RegOpType::WriteReg) {
        if (offset < CP_GLB_REG_SIZE) {
            switch (offset) {
                case CP_GLB_CTRL: {
                    HandleRegWriteCP_GLB_CTRL(pkt->write_value);
                    break;
                }
                default: {
                    assert("offset don't match any cp base reg");
                    break;
                }
            }
        } else if (offset < CP_MAX_QUEUE_NUM * CP_QUEUE_REG_SIZE ) {
            offset -= CP_GLB_REG_SIZE * 4;
            uint32_t queue_id = offset / CP_QUEUE_REG_SIZE;
            offset %= CP_QUEUE_REG_SIZE;
            offset = offset >> 2;
            switch (offset) {
#define WRITE_CP_QUEUE_REG(regname) \
            case regname : {         \
                queue_regs[queue_id][regname] = pkt->write_value; \
                break;              \
            }
                WRITE_CP_QUEUE_REG(CP_QUEUE_CTRL);
                WRITE_CP_QUEUE_REG(CP_QUEUE_RB_BASE_HI);
                WRITE_CP_QUEUE_REG(CP_QUEUE_RB_BASE_LO);
                WRITE_CP_QUEUE_REG(CP_QUEUE_RB_SIZE);
                WRITE_CP_QUEUE_REG(CP_QUEUE_RB_RPTR_HI);
                WRITE_CP_QUEUE_REG(CP_QUEUE_RB_RPTR_LO);
                WRITE_CP_QUEUE_REG(CP_QUEUE_RB_WPTR_HI);
                WRITE_CP_QUEUE_REG(CP_QUEUE_RB_WPTR_LO);
                WRITE_CP_QUEUE_REG(CP_QUEUE_DOORBELL_BASE_HI);
                WRITE_CP_QUEUE_REG(CP_QUEUE_DOORBELL_BASE_LO);
                WRITE_CP_QUEUE_REG(CP_QUEUE_DOORBELL_OFFSET);
                default: {
                    assert("offset don't match any queue base reg");
                    break;
                }
            }
            reg_CP_QUEUE_CTRL* ctrl = reinterpret_cast<reg_CP_QUEUE_CTRL*>(&(queue_regs[queue_id][CP_QUEUE_CTRL]));
            if (offset == CP_QUEUE_CTRL && (ctrl->bits.enable == 1)) {
                auto itr = m_cmd_queues.find(queue_id);
                if (itr != m_cmd_queues.end()){
                    m_cmd_queues[queue_id]->Destroy();
                }
                auto new_queue = make_shared<CommandQueue>(queue_id,
                        PackHighLow(queue_regs[queue_id][CP_QUEUE_DOORBELL_BASE_HI], queue_regs[queue_id][CP_QUEUE_DOORBELL_BASE_LO]),
                        queue_regs[queue_id][CP_QUEUE_DOORBELL_OFFSET],
                        PackHighLow(queue_regs[queue_id][CP_QUEUE_RB_BASE_HI], queue_regs[queue_id][CP_QUEUE_RB_BASE_LO]),
                        queue_regs[queue_id][CP_QUEUE_RB_SIZE],
                        PackHighLow(queue_regs[queue_id][CP_QUEUE_RB_RPTR_HI], queue_regs[queue_id][CP_QUEUE_RB_RPTR_LO]),
                        PackHighLow(queue_regs[queue_id][CP_QUEUE_RB_WPTR_HI], queue_regs[queue_id][CP_QUEUE_RB_WPTR_LO]));
                m_cmd_queues.insert(std::make_pair(queue_id, new_queue));
            }
        }
    }

    PushResp(msg, make_shared<RespDoneMessage>(msg));
}

void StreamProcessor::HandleRegWriteCP_GLB_CTRL(uint32_t val)  {
     m_ctrl.val = val;
     if (m_ctrl.bits.enable == 1) {
         m_status.bits.active = 1;
     } else {
         m_status.bits.active = 0;
     }
}


