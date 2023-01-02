#ifndef _FISHER_YATES_H
#define _FISHER_YATES_H

#include <vector>

namespace k2cache{
std::vector<uint64_t> fisher_yates(uint64_t result_size,
                                        uint64_t choice_set_size);
}

#endif
