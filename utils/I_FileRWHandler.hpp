//
// Created by cristobal on 9/6/21.
//

#ifndef RDFCACHEK2_I_FILERWHANDLER_HPP
#define RDFCACHEK2_I_FILERWHANDLER_HPP

#include "I_IOStream.hpp"
#include "I_IStream.hpp"
#include "I_OStream.hpp"
#include <iostream>
#include <memory>
namespace k2cache {
class I_FileRWHandler {
public:
  virtual ~I_FileRWHandler() = default;

  virtual std::unique_ptr<I_OStream>
  get_writer(std::ios::openmode openmode) = 0;
  virtual std::unique_ptr<I_IStream>
  get_reader(std::ios::openmode openmode) = 0;
  virtual bool exists() = 0;
  virtual std::unique_ptr<I_OStream>
  get_writer_temp(std::ios::openmode openmode) = 0;
  virtual void commit_temp_writer() = 0;
  virtual std::unique_ptr<I_IOStream>
  get_reader_writer(std::ios::openmode openmode) = 0;
  virtual void clean() = 0;
};
} // namespace k2cache
#endif // RDFCACHEK2_I_FILERWHANDLER_HPP
