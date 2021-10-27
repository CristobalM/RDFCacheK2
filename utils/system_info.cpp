//
// Created by cristobal on 23-10-21.
//

#include <string>
#include <fstream>
#include <limits>
#include "system_info.hpp"

unsigned long get_mem_total() {
    std::string token;
    std::ifstream file("/proc/meminfo");
    while (file >> token) {
        if (token == "MemTotal:") {
            unsigned long mem;
            if (file >> mem) {
                return mem;
            } else {
                return 0;
            }
        }
        // ignore rest of the line
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return 0; // nothing found
}
