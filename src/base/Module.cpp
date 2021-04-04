#include "inc/Module.h"
#include "inc/Engine.h"
#include "common/utils.h"
// #include "dmlc/logging.h"

using namespace model;

void Module::FetchInput() {
    // if (!(m_msg_active_list->Empty()))
    //    return;

    MsgPtr msg = PopQueue();
    if (msg) {
        m_msg_active_list.push_back(msg);
    }

    if (!m_response_queue->Empty()) {
        msg = m_response_queue->WaitAndPop();
        m_msg_response_list.push_back(msg);
        // m_msg_active_list.push_back(msg);
    }
};

bool Module::Execute() {

    if (m_msg_active_list.empty()) {
        if (!m_msg_pending_list.empty()) {
            m_msg_active_list.assign(m_msg_pending_list.begin(), m_msg_pending_list.end());
            m_msg_pending_list.clear();
        } else {
            return false;
        }
    }

    // FIXME is it erase correct in for loop
    for (auto msg = m_msg_active_list.begin(); msg != m_msg_active_list.end(); ) {
        // shared_ptr<Message> msg = this->m_msg_active_list->front();
        switch ((*msg)->GetMessageType()) {
            case MessageType::RESP_DONE: {
                auto ref_msg = (*msg)->GetRefMessage();
                bool response = false;
                for (auto resp_wait_msg: m_msg_response_list) {
                    // TODO check == works
                    if (resp_wait_msg == ref_msg) {
                        resp_wait_msg->SetRespDone();
                        msg = m_msg_active_list.erase(msg);
                        ProcessRespMsg(resp_wait_msg);
                        response = true;
                        break;
                    }
                }
                if (response ==false) {
                    m_msg_pending_list.push_back(ref_msg);
                    msg++;
                }
                break;
            }
            case MessageType::REG_REQ: {
                ProcessRegAccess(*msg);
                msg = m_msg_active_list.erase(msg);
                break;
            }
            default: {
                msg++;
                break;
            }
        }
    }

    return true;
};

// TODO change to input?
bool Module::IsQueueEmpty(const std::string &name) {
    auto it =m_output_queue.find(name);
    assert(it != m_output_queue.end()) ;

    auto msg_queue = it->second;

    return msg_queue->Empty();
}

void Module::AddInputQueue(const std::string& from, MsgQueuePtr msg_queue) {
    m_input_queue.insert(make_pair(from, msg_queue));
};

void Module::AddOutputQueue(const std::string& to, MsgQueuePtr msg_queue) {
    m_output_queue.insert(make_pair(to, msg_queue));
};


void Module::PushReq(const std::string &name, const MsgPtr msg) {
    auto msg_queue = m_engine->GetReqQueue(m_name, name);
    // auto msg_resp_queue = msg->GetRespQueue();
    debug_print("Message %s to Queue %s\n", msg->Name().c_str(), msg_queue->Name().c_str());

    msg->SetRespQueue(m_engine->GetRespQueue(m_name, name));
    msg_queue->Push(msg);
}

void Module::PushResp(const MsgPtr req_msg, const MsgPtr msg) {
    auto msg_queue = req_msg->GetRespQueue();
    debug_print("Message %s to Queue %s\n", msg->Name().c_str(), msg_queue->Name().c_str());
    msg_queue->Push(msg);
}

void Module::PushResp(const std::string &name, const MsgPtr msg) {
    auto msg_queue = m_engine->GetRespQueue(name, m_name);
    debug_print("Message %s to Queue %s\n", msg->Name().c_str(), msg_queue->Name().c_str());
    /*
    auto it = m_output_queue.find(name);
    CHECK(it != m_output_queue.end()) ;
    auto msg_queue = it->second;
    */
    msg_queue->Push(msg);
}

MsgPtr Module::FrontReq(const std::string &name) {
    /*
    auto it = m_input_queue.find(name);
    CHECK(it != m_input_queue.end()) ;

    auto msg_queue = it->second;
    */
    auto msg_queue = m_engine->GetReqQueue(name, m_name);
    if (msg_queue->Empty())
        return nullptr;
    return msg_queue->Front();
}

MsgPtr Module::FrontResp(const std::string &name) {
    /*
    auto it = m_input_queue.find(name);
    CHECK(it != m_input_queue.end()) ;

    auto msg_queue = it->second;
    */
    auto msg_queue = m_engine->GetRespQueue(m_name, name);
    if (msg_queue->Empty())
        return nullptr;
    return msg_queue->Front();
}

MsgPtr Module::PopReq(const std::string &name) {
    auto msg_queue = m_engine->GetReqQueue(name, m_name);
    /*
    auto it = m_input_queue.find(name);
    CHECK(it != m_input_queue.end()) ;
    */

    if (msg_queue->Empty())
        return nullptr;
    shared_ptr<Message> msg = msg_queue->WaitAndPop();
    return msg;
}

MsgPtr Module::PopResp(const std::string &name) {
    auto msg_queue = m_engine->GetReqQueue(m_name, name);
    /*
    auto it = m_input_queue.find(name);
    CHECK(it != m_input_queue.end()) ;
    */

    if (msg_queue->Empty())
        return nullptr;
    shared_ptr<Message> msg = msg_queue->WaitAndPop();
    return msg;
}


MsgPtr Module::PopQueue() {
    for (auto it = m_input_queue.cbegin(); it != m_input_queue.cend(); ++it ) {
        auto msg_queue = it->second;

        if (msg_queue->Empty())
            continue;

        return msg_queue->WaitAndPop();
    }
    return nullptr;
}

