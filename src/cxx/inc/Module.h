#pragma once
#include <string>
#include <list>

#include "inc/Message.h"
#include "inc/MsgQueue.h"
#include "inc/Model.h"

using namespace std;

namespace model {

class Module;
class Model;

using ModulePtr = std::shared_ptr<Module>;
using Connection = std::pair<MsgQueuePtr, MsgQueuePtr>;
using ConnectionPtr = shared_ptr<Connection>;


class Module : public ClockedObject {
    public:
        Module(const ModuleParams* p);

        virtual ~Module() {}

        const std::string &Name() { return m_name;}

        void AddInputQueue(const std::string& from, MsgQueuePtr msg_queue) ;

        void AddOutputQueue(const std::string& to, MsgQueuePtr msg_queue);

        // return true if not idle, otherwise return false
        virtual bool Execute();

        virtual void FetchInput();

        virtual void ProcessRespMsg(MsgPtr) {};
        virtual void ProcessRegAccess(MsgPtr) {};

        // Process on teration of the module
        // it fetcvh input from input queue and put the output in output queue
        bool Process() {
            FetchInput();
            bool busy = Execute();
            return busy;
        }

        // TDO virtual void HandleRegisterAccess(shared_ptr<Message> msg) = 0;

        bool IsQueueEmpty(const std::string &name) ;

        void PushReq(const std::string &name, const MsgPtr msg) ;
        void PushResp(const std::string &name, const MsgPtr msg) ;
        void PushResp(const MsgPtr req_msg, const MsgPtr msg) ;
        MsgPtr FrontReq(const std::string &name) ;
        MsgPtr FrontResp(const std::string &name) ;
        MsgPtr PopReq(const std::string &name) ;
        MsgPtr PopResp(const std::string &name) ;
        MsgPtr PopQueue() ;


    protected:
        Model* m_model;

        const std::string m_name;

        // input/output queue is push/pop from other module's
        map<string, MsgQueuePtr> m_input_queue;
        map<string, MsgQueuePtr> m_output_queue;
        // its used by nont process function of current module
        MsgQueuePtr     m_response_queue;

        // Process function use it
        std::list<MsgPtr> m_msg_active_list;
        std::list<MsgPtr> m_msg_pending_list;       // msg can't be processed for now
        std::list<MsgPtr> m_msg_response_list;      // msg from m_response_queue

};

Module* ModuleParams::create()
{
    return new Module(this);
}

}
