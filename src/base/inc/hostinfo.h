#pragma once
#include <string>


std::string __get_hostname();

std::string &hostname();

uint64_t procInfo(const char *filename, const char *target);

/**
 * Determine the simulator process' total virtual memory usage.
 *
 * @return virtual memory usage in kilobytes
 */
uint64_t memUsage();

