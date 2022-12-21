#ifndef _FISHER_YATES_H
#define _FISHER_YATES_H

#include <vector>

namespace k2cache{
std::vector<unsigned long> fisher_yates(unsigned long result_size,
                                        unsigned long choice_set_size);
}

#endif
