//
// Created by cristobal on 9/6/21.
//

#ifndef RDFCACHEK2_I_FILERWHANDLER_HPP
#define RDFCACHEK2_I_FILERWHANDLER_HPP

#include <iostream>
#include <memory>
class I_FileRWHandler {
public:
  virtual ~I_FileRWHandler() = default;

  virtual std::unique_ptr<std::ostream>
  get_writer(std::ios::openmode openmode) = 0;
  virtual std::unique_ptr<std::istream>
  get_reader(std::ios::openmode openmode) = 0;
  virtual bool exists() = 0;
  virtual std::unique_ptr<std::ostream>
  get_writer_temp(std::ios::openmode openmode) = 0;
  virtual void commit_temp_writer() = 0;
};

#endif // RDFCACHEK2_I_FILERWHANDLER_HPP
