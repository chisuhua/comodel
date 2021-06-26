/**
 * @file
 *
 *  C++-only configuration and instantiation support.  This allows a
 *  config to be read back from a .ini and instantiated without
 *  Python.  Useful if you want to embed gem5 within a larger system
 *  without carrying the integration cost of the fully-featured
 *  configuration system.
 *
 *  This file contains a demonstration main using ConfigManager.
 *  Build with something like:
 *
 *      scons --without-python build/ARM/libgem5_opt.so
 *
 *      g++ -DTRACING_ON -std=c++0x -Ibuild/ARM src/sim/cxx_main.cc \
 *          -o gem5cxx.opt -Lbuild/ARM -lgem5_opt
 */

#include <cstdlib>
#include <iostream>
#include <sstream>

#include "module/inc/IniFile.h"
// #include "base/inc/statistics.h"
#include "base/inc/str.h"
#include "base/inc/trace.h"
// #include "cpu/base.hh"
#include "module/inc/ConfigIniFile.h"
#include "module/inc/ConfigManager.h"
#include "ClockFreq.h"
// #include "sim/init_signals.hh"
// #include "sim/serialize.hh"
#include "SimEvents.h"
#include "Simulate.h"
// #include "sim/stat_control.hh"
// #include "sim/system.hh"
// #include "stats.hh"

void
usage(const std::string &prog_name)
{
    std::cerr << "Usage: " << prog_name << (
        " <config-file.ini> [ <option> ]\n\n"
        "OPTIONS:\n"
        "    -p <object> <param> <value>  -- set a parameter\n"
        "    -v <object> <param> <values> -- set a vector parameter from"
        " a comma\n"
        "                                    separated values string\n"
        "    -d <flag>                    -- set a debug flag (-<flag>\n"
        "                                    clear a flag)\n"
        "    -s <dir> <ticks>             -- save checkpoint to dir after"
        " the given\n"
        "                                    number of ticks\n"
        "    -r <dir>                     -- restore checkpoint from dir\n"
        "    -c <from> <to> <ticks>       -- switch from cpu 'from' to cpu"
        " 'to' after\n"
        "                                    the given number of ticks\n"
        "\n"
        );

    std::exit(EXIT_FAILURE);
}

int
main(int argc, char **argv)
{
    std::string prog_name(argv[0]);
    unsigned int arg_ptr = 1;

    if (argc == 1)
        usage(prog_name);

    // ConfigInit();

    // initSignals();

    setClockFrequency(1000000000000);
    curEventQueue(getEventQueue(0));

    // Stats::initSimStats();
    // Stats::registerHandlers(CxxConfig::statsReset, CxxConfig::statsDump);

    Trace::enable();
    // setDebugFlag("Terminal");
    // setDebugFlag("CxxConfig");

    const std::string config_file(argv[arg_ptr]);

    ConfigIniFile *conf = new ConfigIniFile();

    if (!conf->load(config_file.c_str())) {
        std::cerr << "Can't open config file: " << config_file << '\n';
        return EXIT_FAILURE;
    }
    arg_ptr++;

    ConfigManager *config_manager = new ConfigManager(*conf);

    Tick pre_run_time = 1000000;
    Tick pre_switch_time = 1000000;

    try {
        while (arg_ptr < argc) {
            std::string option(argv[arg_ptr]);
            arg_ptr++;
            unsigned num_args = argc - arg_ptr;

            if (option == "-p") {
                if (num_args < 3)
                    usage(prog_name);
                config_manager->setParam(argv[arg_ptr], argv[arg_ptr + 1],
                    argv[arg_ptr + 2]);
                arg_ptr += 3;
            } else if (option == "-v") {
                std::vector<std::string> values;

                if (num_args < 3)
                    usage(prog_name);
                tokenize(values, argv[arg_ptr + 2], ',');
                config_manager->setParamVector(argv[arg_ptr],
                    argv[arg_ptr + 1], values);
                arg_ptr += 3;
            } else {
                usage(prog_name);
            }
        }
    } catch (ConfigManager::Exception &e) {
        std::cerr << e.name << ": " << e.message << "\n";
        return EXIT_FAILURE;
    }

    // CxxConfig::statsEnable();
    getEventQueue(0)->dump();

    try {
        config_manager->instantiate();
        config_manager->initState();
        config_manager->startup();
    } catch (ConfigManager::Exception &e) {
        std::cerr << "Config problem in sim object " << e.name
            << ": " << e.message << "\n";

        return EXIT_FAILURE;
    }

    GlobalSimLoopExitEvent *exit_event = NULL;

    exit_event = simulate();

    std::cerr << "Exit at tick " << curTick()
        << ", cause: " << exit_event->getCause() << '\n';

    getEventQueue(0)->dump();

#if TRY_CLEAN_DELETE
    config_manager->deleteObjects();
#endif

    delete config_manager;

    return EXIT_SUCCESS;
}
