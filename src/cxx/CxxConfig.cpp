
#include "sim/cxx_config.hh"

const std::string CxxConfigParams::invalidName = "<invalid>";

/** Directory of all SimObject classes config details */
std::map<std::string, CxxConfigDirectoryEntry *> cxx_config_directory;
