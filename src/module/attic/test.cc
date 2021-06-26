#include <tvm/runtime/module.h>
#include <tvm/runtime/packed_func.h>
#include <tvm/runtime/registry.h>

int main(void) {
  tvm::runtime::Module mod = tvm::runtime::Module::LoadFromFile("lib/plugin_module.so");
  tvm::runtime::PackedFunc f_AddOne = mod.GetFunction("AddOne");
  int ret = f_AddOne(10);
  assert(ret == 11);

  tvm::runtime::PackedFunc f_CreateMyModule = mod.GetFunction("CreateMyModule");
  tvm::runtime::Module mymod = f_CreateMyModule(10);
  tvm::runtime::PackedFunc f_add = mymod.GetFunction("add");
  int ret2 = f_add(10);
  assert(ret2 == 20);
  return 0;
}
