project(
  'mesa',
  ['c', 'cpp'],
  version : run_command(
    [find_program('python', 'python3'), 'bin/meson_get_version.py']
  ).stdout(),
  license : 'MIT',
  meson_version : '>= 0.52',
  default_options : ['buildtype=debug', 'b_ndebug=if-release', 'c_std=c99', 'cpp_std=c++14']
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

extra_cpp_args = ['-Wall', '-Werror']

if buildtype == 'release'
  extra_cpp_args += ['-O3', '-g', '-Wno-strict-aliasing', '-msse4.1', '-flto']
endif

extra_c_args = extra_cpp_args

if cpp.get_id() == 'gcc'
  extra_cpp_args += ['-std=c++11']
else
  extra_cpp_args += []
endif

link_args = ['-ldl', '-lpthread', '-L' + meson.current_build_dir(), '-lstdc++'
  ]

model_symbol_list = meson.current_source_dir() + '/model_symbol_export.def'

#pymodule = import('python3')
#python3 = pymodule.find_python()

costream_home = run_command([find_program('python', 'python3'), '-c', 'import os; print(os.environ["COSTREAM_HOME"])']).stdout().strip()
codriver_home = run_command([find_program('python', 'python3'), '-c', 'import os; print(os.environ["CODRIVER_HOME"])']).stdout().strip()
coasm_home = run_command([find_program('python', 'python3'), '-c', 'import os; print(os.environ["COASM_HOME"])']).stdout().strip()
#env = environment()
#costream_home = env.get('COSTREAM_HOME')
#codriver_home = env.get('CODRIVER_HOME')
message('COSTREAM_HOME is ' + costream_home)
message('CODRIVER_HOME is ' + codriver_home)
message('COASM_HOME is ' + coasm_home)
#if res.returncode() == 0
#    costream_inc = res.stdout
#else
costream_inc = [costream_home + '/pps/platform',
  costream_home + '/pps/device',
  costream_home + '/cmdio',
  codriver_home + '/common'
  ]

message(costream_inc)

model_inc = ['src']

model_base_inc = ['src/base']
model_base_src = [
  'src/base/Model.cpp',
  'src/base/Module.cpp'
  ]

behavior_link_args = link_args + ['-Wl,--version-script='+model_symbol_list]
behavior_inc = ['src/behavior']
behavior_src = [
  'src/behavior/Compute.cpp',
  'src/behavior/ComputeUnit.cpp',
  'src/behavior/CopyEngine.cpp',
  'src/behavior/StreamProcessor.cpp',
  'src/behavior/Engine.cpp',
  'src/behavior/Memory.cpp',
  'src/behavior/MemoryImpl.cc',
  'src/behavior/Mmio.cpp',
  'src/behavior/ThreadItem.cpp',
  'src/behavior/Warp.cpp',
  'src/behavior/Instruction.cpp',
  'src/behavior/InstructionLDS.cpp',
  'src/behavior/InstructionFLAT.cpp',
  #'src/behavior/InstructionMTBUF.cpp',
  'src/behavior/InstructionMUBUF.cpp',
  'src/behavior/InstructionSMRD.cpp',
  'src/behavior/InstructionSOP1.cpp',
  'src/behavior/InstructionSOP2.cpp',
  'src/behavior/InstructionSOPC.cpp',
  'src/behavior/InstructionSOPK.cpp',
  'src/behavior/InstructionSOPP.cpp',
  'src/behavior/InstructionVOP1.cpp',
  'src/behavior/InstructionVOP2.cpp',
  'src/behavior/InstructionVOP3A.cpp',
  'src/behavior/InstructionVOP3B.cpp',
  'src/behavior/InstructionVOPC.cpp'
  ]

behavior_src += model_base_src
behavior_inc += model_base_inc
behavior_inc += model_inc
behavior_inc += costream_inc
behavior_inc += coasm_home

behavior = shared_library(
  'bhmodel',
  behavior_src,
  include_directories : include_directories(behavior_inc),
  install : false,
  cpp_args : extra_cpp_args,
  link_args : behavior_link_args,
  )