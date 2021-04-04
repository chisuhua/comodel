#include "inc/Model.h"

#ifdef __cplusplus
extern "C" {
#endif

void* create_model(uint32_t deviceIdx) {
    Model* pModel = new Model(deviceIdx);
    return reinterpret_cast<void*>(pModel);
}

void destroy_model(void* handle) {
    Model* pModel = reinterpret_cast<Model*>(handle);
    pModel->Destroy();
}

void read_mmio(void* handle, uint32_t offset, void* data, uint32_t length, uint32_t bar) {
    Model* pModel = reinterpret_cast<Model*>(handle);
    pModel->ReadMmio(offset, data, length, bar);
}

void write_mmio(void* handle, uint32_t offset, void* data, uint32_t length, uint32_t bar) {
    Model* pModel = reinterpret_cast<Model*>(handle);
    pModel->WriteMmio(offset, data, length, bar);
}

void write_register(void* handle, uint32_t offset, uint32_t value){
    Model* pModel = reinterpret_cast<Model*>(handle);
    pModel->WriteRegister(offset, value);
}

uint32_t read_register(void* handle, uint32_t offset){
    Model* pModel = reinterpret_cast<Model*>(handle);
    return pModel->ReadRegister(offset);
}

uint32_t get_device_cnt(){
    ModelConfig model_config;
    return model_config.GetDeviceConfigs().size();
}

void query_adapter_info(uint32_t deviceIdx, AdapterInfo* pInfo) {
    ModelConfig  model_config;
    auto& configs = model_config.GetDeviceConfigs();
    pInfo->deviceId = configs[deviceIdx].id;
    pInfo->fbSize = 0x100000000;
    pInfo->vaStart = 0;
    pInfo->vaEnd   = (0x1UL << 49) - 1;
    pInfo->vaBaseAlign = 64;
}


#ifdef __cplusplus
}
#endif


Model::Model(uint32_t deviceIdx) :
    m_engine(deviceIdx)
{
}

uint32_t Model::ReadRegister(uint32_t offset) {
    return m_engine.ReadRegister(offset);
}

void Model::WriteRegister(uint32_t offset, uint32_t value) {
    m_engine.WriteRegister(offset, value);
}

void Model::WriteMmio(uint32_t offset, void* data, uint32_t length, uint32_t bar) {
    m_engine.WriteMmio(offset, data, length, bar);
}

void Model::ReadMmio(uint32_t offset, void* data, uint32_t length, uint32_t bar) {
    m_engine.ReadMmio(offset, data, length, bar);
}

void Model::Destroy() {
    m_engine.Stop();
}


