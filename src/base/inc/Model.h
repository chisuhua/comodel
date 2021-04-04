#pragma once
#include <inttypes.h>
#include "inc/Engine.h"

using namespace model;

class Model {
    public:
        Model(uint32_t deviceIdx);
       ~Model() {}
        void Destroy();
        uint32_t ReadRegister(uint32_t offset);
        void WriteRegister(uint32_t offset, uint32_t value);
        void WriteMmio(uint32_t offset, void* data, uint32_t length, uint32_t bar);
        void ReadMmio(uint32_t offset, void* data, uint32_t length, uint32_t bar);
        Engine m_engine;
};

#define DEVICE_ID 0xdeadbeaf
struct AdapterInfo {
    uint32_t deviceId;
    uint64_t fbSize;
    uint32_t numPTEs;
    uint64_t vaStart;
    uint64_t vaEnd;
    uint32_t vaBaseAlign;
};

struct DeviceConfig {
    uint32_t id;
    uint32_t vf_id;
    uint32_t vf_ce_mask;
    uint32_t vf_llc_mask;
    uint32_t vf_queue_mask;
};


struct ModelConfig {
    ModelConfig() {
        DeviceConfig dev_cfg;
        dev_cfg.id = DEVICE_ID;
        dev_cfg.vf_id = 1;
        dev_cfg.vf_ce_mask = 0xFF;
        dev_cfg.vf_llc_mask = 0xFF;
        dev_cfg.vf_queue_mask = 0xFF;
        m_dev_cfg.push_back(dev_cfg);
    };

    ~ModelConfig() {};

    const std::vector<DeviceConfig>& GetDeviceConfigs() { return m_dev_cfg; }

    std::vector<DeviceConfig> m_dev_cfg;
};





