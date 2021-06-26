project(
  'comdel',
  ['c', 'cpp'],
  version : run_command(
    [find_program('python', 'python3'), 'bin/meson_get_version.py']
  ).stdout(),
  license : 'MIT',
  meson_version : '>= 0.52',
  default_options : ['buildtype=debug'] #, 'b_ndebug=if-release', 'c_std=c99', 'cpp_std=c++17']
)

cc = meson.get_compiler('c')
cpp = meson.get_compiler('cpp')
buildtype = get_option('buildtype')
builddir = meson.current_build_dir()

null_dep = dependency('', required : false)

# Arguments for the preprocessor, put these in a separate array from the C and
# C++ (cpp in meson terminology) arguments since they need to be added to the
# default arguments for both C and C++.
pre_args = [
  '-D__STDC_CONSTANT_MACROS',
  '-D__STDC_FORMAT_MACROS',
  '-D__STDC_LIMIT_MACROS',
  '-DPACKAGE_VERSION="@0@"'.format(meson.project_version()),
  '-DPACKAGE_BUGREPORT="https://gitlab.freedesktop.org/mesa/mesa/-/issues"',
]

extra_cpp_args = ['-Wall', '-std=c++17']

if buildtype == 'release'
  extra_cpp_args += ['-O3', '-g', '-Wno-strict-aliasing', '-msse4.1', '-flto']
endif

extra_c_args = extra_cpp_args

#if cpp.get_id() == 'gcc'
#  extra_cpp_args += ['-std=c++17']
#else
#  extra_cpp_args += []
#endif

link_args = ['-ldl', '-lpthread', '-L' + meson.current_build_dir(), '-lstdc++' ]

model_symbol_list = meson.current_source_dir() + '/model_symbol_export.def'

#pymodule = import('python3')
#python3 = pymodule.find_python()

COSTREAM_HOME = run_command([find_program('python', 'python3'), '-c', 'import os; print(os.environ["COSTREAM_HOME"])']).stdout().strip()
CODRIVER_HOME = run_command([find_program('python', 'python3'), '-c', 'import os; print(os.environ["CODRIVER_HOME"])']).stdout().strip()
COASM_HOME = run_command([find_program('python', 'python3'), '-c', 'import os; print(os.environ["COASM_HOME"])']).stdout().strip()
TVM_HOME = run_command([find_program('python', 'python3'), '-c', 'import os; print(os.environ["TVM_HOME"])']).stdout().strip()
LIBGO_HOME = run_command([find_program('python', 'python3'), '-c', 'import os; print(os.environ["LIBGO_HOME"])']).stdout().strip()
IGUANA_HOME = run_command([find_program('python', 'python3'), '-c', 'import os; print(os.environ["IGUANA_HOME"])']).stdout().strip()

message('COSTREAM_HOME is ' + COSTREAM_HOME)
message('CODRIVER_HOME is ' + CODRIVER_HOME)
message('COASM_HOME is ' + COASM_HOME)
message('TVM_HOME is ' + TVM_HOME)
message('IGUANA_HOME is ' + IGUANA_HOME)

#costream_inc = [COSTREAM_HOME + '/pps/platform',
#  COSTREAM_HOME + '/pps/device',
#  COSTREAM_HOME + '/cmdio',
#  CODRIVER_HOME + '/common'
#  ]

costream_inc = [COSTREAM_HOME + '/include',
  CODRIVER_HOME + '/common'
  ]



message(costream_inc)

model_inc = ['src']

model_base_inc = ['src/oldmodule']
model_base_src = [
  'src/oldmodule/Model.cpp',
  'src/oldmodule/Module.cpp'
  ]

behavior_link_args = link_args + ['-Wl,--version-script='+model_symbol_list]
behavior_inc = ['src/behavior/gpu']
behavior_src = [
  'src/behavior/gpu/Compute.cpp',
  'src/behavior/gpu/ComputeUnit.cpp',
  'src/behavior/gpu/CopyEngine.cpp',
  'src/behavior/gpu/StreamProcessor.cpp',
  'src/behavior/gpu/Engine.cpp',
  'src/behavior/gpu/Memory.cpp',
  'src/behavior/gpu/MemoryImpl.cc',
  'src/behavior/gpu/Mmio.cpp',
  'src/behavior/gpu/ThreadItem.cpp',
  'src/behavior/gpu/Warp.cpp',
  'src/behavior/gpu/Instruction.cpp',
  'src/behavior/gpu/InstructionLDS.cpp',
  'src/behavior/gpu/InstructionFLAT.cpp',
  #'src/behavior/gpu/InstructionMTBUF.cpp',
  'src/behavior/gpu/InstructionMUBUF.cpp',
  'src/behavior/gpu/InstructionSMRD.cpp',
  'src/behavior/gpu/InstructionSOP1.cpp',
  'src/behavior/gpu/InstructionSOP2.cpp',
  'src/behavior/gpu/InstructionSOPC.cpp',
  'src/behavior/gpu/InstructionSOPK.cpp',
  'src/behavior/gpu/InstructionSOPP.cpp',
  'src/behavior/gpu/InstructionVOP1.cpp',
  'src/behavior/gpu/InstructionVOP2.cpp',
  'src/behavior/gpu/InstructionVOP3A.cpp',
  'src/behavior/gpu/InstructionVOP3B.cpp',
  'src/behavior/gpu/InstructionVOPC.cpp'
  ]

behavior_src += model_base_src
behavior_inc += model_base_inc
behavior_inc += model_inc
behavior_inc += costream_inc
behavior_inc += COASM_HOME

behavior = shared_library(
  'bhmodel',
  behavior_src,
  include_directories : include_directories(behavior_inc),
  install : false,
  cpp_args : extra_cpp_args,
  link_args : behavior_link_args,
  )

module_inc = ['src',
  'src/common',
  'src/module/inc',
  'src/sim/inc'
  ]

module_inc += [TVM_HOME + '/include']
module_inc += [TVM_HOME + '/3rdparty/dmlc-core/include']
module_inc += [TVM_HOME + '/3rdparty/dlpack/include']
module_inc += [IGUANA_HOME]

module_link_args = link_args + ['-Wl,--version-script='+model_symbol_list]
module_src = [
  'src/module/IniFile.cpp',
  'src/module/Module.cpp',
  'src/module/ConfigIniFile.cpp',
  'src/module/ConfigManager.cpp',
  'src/base/str.cpp',
  'src/base/logging.cpp',
  'src/base/hostinfo.cpp',
  'src/module/tvm_runtime_pack.cc',
  ]

module = library(
  'module',
  module_src,
  include_directories : include_directories(module_inc),
  install : false,
  cpp_args : extra_cpp_args,
  link_args : module_link_args,
  )

test_simple_src = [
  'test/main.cpp',
  'test/module/Simple.cpp'
  ]

test_simple = executable(
  'test_simple',
  test_simple_src,
  include_directories : include_directories(module_inc),
  install : false,
  cpp_args : extra_cpp_args,
  link_args : module_link_args,
  #dependencies: declare_dependency(link_with : )
  )

