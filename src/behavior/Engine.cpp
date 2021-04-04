#include "inc/Engine.h"
#include "inc/Memory.h"
#include "inc/StreamProcessor.h"
#include "inc/ComputeUnit.h"
#include "inc/CopyEngine.h"
#include "inc/Mmio.h"

using namespace model;

Engine::Engine(uint32_t device_num): ready_(false) {
    // 1GB fb size
    const uint64_t fb_size = 0x40000000;  // model_cmdio.cpp set pa_range_top
    // m_svm_mem = new MemorySVMImpl("global");
    m_global_mem = new MemoryImpl<4096*1024>("global", 32 * 1024);
    // create memory
    // m_mc = make_shared<Memory>(this, fb_size);

    // create cp
    // m_cp = make_shared<StreamProcessor>(this);
    // m_ce = make_shared<CopyEngine>(this);

    // Note: we use Module's name as instance name
    m_mc = CreateModule<Memory>("", fb_size);
    m_cp = CreateModule<StreamProcessor>("");
    m_ce = CreateModule<CopyEngine>("");
    m_mmio = CreateModule<Mmio>("");

    // uint32_t vf_active_cu_mask = GetVfActiveCuMask();
    // uint32_t vf_id = GetActiveVfId();


    // setp queue for cp
    // m_engine_cp_queue = make_shared<MsgQueuePtr>();
    // m_cp->AddInputQueue("engine_", m_engine_cp_queue);
    Connect(m_mmio, m_cp);
    Connect(m_mmio, m_mc);
    Connect(m_cp, m_mc);

    for (uint32_t i = 0; i < CU_NUM; i++) {
        m_cu[i] = CreateModule<ComputeUnit>(std::to_string(i));

        // cp cu queue
        Connect(m_cp, m_cu[i]);

        // cu mc
        Connect(m_cu[i], m_mc);
        //m_cu_mc_req_queue[i] = make_shared<MsgQueuePtr>();
        // m_cu_mc_resp_queue[i] = make_shared<MsgQueuePtr>();

        // m_mc->AddInputQueue("cu" + std::to_string(i),  m_cu_mc_req_queue[i]);
        // m_cu[i]->AddOutputQueue("mc",                  m_cu_mc_req_queue);
        // m_mc->AddOutputQueue("cu" + std::to_string(i), m_cu_mc_resp_queue[i]);
        // m_cu[i]->AddOutputQueue("cp",                  m_cu_mc_resp_queue);
    }

    // cp ce
    Connect(m_cp, m_ce);

    Connect(m_ce, m_mc);
    /*
    m_cp_ce_req_queue = make_shared<MsgQueuePtr>();
    m_cp_ce_resp_queue = make_shared<MsgQueuePtr>();

    m_cp->AddOutputQueue("ce", m_cp_ce_req_queue);
    m_ce->AddInputQueue("cp",  m_cp_ce_req_queue_);
    m_cp->AddInputQueue("ce",  m_cp_ce_resp_queue);
    m_ce->AddOutputQueue("cp", m_cp_ce_resp_queue);

    // ce mc
    m_ce_mc_req_queue = make_shared<MsgQueuePtr>();
    m_ce_mc_resp_queue = make_shared<MsgQueuePtr>();

    m_ce->AddOutputQueue("mc", m_ce_mc_req_queue);
    m_mc->AddInputQueue("ce",  m_ce_mc_req_queue_);
    m_ce->AddInputQueue("mc",  m_ce_mc_resp_queue);
    m_mc->AddOutputQueue("ce", m_ce_mc_resp_queue);
    */
    m_engine_thread = unique_ptr<thread>(new thread(&Engine::Run, this));
    auto handle = m_engine_thread->native_handle();
    pthread_setname_np(handle, "Core Engine Thread");
}


void Engine::Run() {
    bool busy = true;
    while(busy) {
        busy = false;
        busy |= m_mmio->Process();   // mmio always return busy until receive set stop reg
        busy |= m_cp->Process();
        busy |= m_ce->Process();
        busy |= m_mc->Process();

        for (uint32_t i = 0; i< CU_NUM; i++) {
            busy |= m_cu[i]->Process();
        }
    }
    printf("Engine stop running\n");
}

void Engine::WriteMmio(uint32_t offset, void* data, uint32_t length, uint32_t bar) {
    m_mmio->Write(offset, data, length, bar);
}

void Engine::ReadMmio(uint32_t offset, void* data, uint32_t length, uint32_t bar) {
    m_mmio->Read(offset, data, length, bar);
}

void Engine::WriteRegister(uint32_t offset, uint32_t value) {
    m_mmio->WriteRegister(offset, value);
}

uint32_t Engine::ReadRegister(uint32_t offset) {
    uint32_t value;
    m_mmio->ReadRegister(offset, &value);
    return value;
}

void Engine::Stop() {
    // call MMio to stop
    /*
    shared_ptr<Message> msg(new Message(MessageType::STOP));
    m_engine_queue.Push(msg);
    */
}

// we hard code  the queue name is "from_instance_name"_"to_instance_name"_req/resp
void Engine::Connect_(ModulePtr from, ModulePtr to) {
    MsgQueuePtr req_queue = make_shared<MsgQueue<MsgPtr>>();
    MsgQueuePtr resp_queue = make_shared<MsgQueue<MsgPtr>>();
    // req_queue->SetName(from->Name() + "_" + to->Name() + "_req");
    // resp_queue->SetName(from->Name() + "_" + to->Name() + "_resp");

    std::string conn_name(from->Name() + "_" + to->Name());

    ConnectionPtr conn = make_shared<Connection>(make_pair(req_queue, resp_queue));

    m_connections.insert(make_pair(conn_name, conn));

    req_queue->SetName(conn_name);
    resp_queue->SetName(conn_name);

    from->AddOutputQueue(to->Name(), req_queue);
    to->AddInputQueue(from->Name(), req_queue);

    from->AddInputQueue(to->Name(), resp_queue);
    to->AddOutputQueue(from->Name(), resp_queue);
}

ConnectionPtr Engine::GetConnect(const std::string& from_name, const std::string& to_name) {
    auto it = m_connections.find(from_name + "_" + to_name);
    if (it != m_connections.end()) {
        return it->second;
    }
    assert(0);
    return nullptr;
    // TODO LOG_ERROR;
}

MsgQueuePtr Engine::GetReqQueue(const std::string& from_name, const std::string& to_name) {
    return GetConnect(from_name, to_name)->first;
}

MsgQueuePtr Engine::GetRespQueue(const std::string& from_name, const std::string& to_name) {
    return GetConnect(from_name, to_name)->second;
}
