//
// Created by cristobal on 9/6/21.
//

#ifndef RDFCACHEK2_FILERWHANDLER_HPP
#define RDFCACHEK2_FILERWHANDLER_HPP

#include <I_FileRWHandler.hpp>
#include <memory>
class FileRWHandler : public I_FileRWHandler {
  std::string filename;
public:
  explicit FileRWHandler(std::string filename);
  std::unique_ptr<std::ostream>
  get_writer(std::ios::openmode openmode) override;
  std::unique_ptr<std::istream>
  get_reader(std::ios::openmode openmode) override;
  bool exists() override;
  std::unique_ptr<std::ostream>
  get_writer_temp(std::ios::openmode openmode) override;
  void commit_temp_writer() override;
};

#endif // RDFCACHEK2_FILERWHANDLER_HPP
