#ifndef _SD_BUILDER_HPP_
#define _SD_BUILDER_HPP_

#include <istream>
#include <memory>

#include <StringDictionary.h>

struct SDInput {
  int thread_count;
  unsigned long cut_size;
  int bucket_size;
};

class SDBuilder {
public:
  enum SDType {
    PFC = 0,
    HTFC = 1,
    HRPDAC = 2,
    RPDAC = 3,
    HRPDACBlocks = 4,
  };

  SDBuilder(
    SDType sd_type,
    bool base_64_data,
    SDInput extra_input
  );

  std::unique_ptr<StringDictionary> build(std::istream &input_stream);

private:
  SDType sd_type;
  bool base_64_data;
  SDInput extra_input;
};

#endif /* _SD_BUILDER_HPP_ */