//
// Created by cristobal on 9/7/21.
//

#ifndef RDFCACHEK2_FHMOCK_HPP
#define RDFCACHEK2_FHMOCK_HPP

#include <ios>
#include <memory>
#include <string>

#include <I_FileRWHandler.hpp>
#include <I_IStream.hpp>
#include <I_OStream.hpp>

struct FHMock : public I_FileRWHandler {
  std::string data;
  std::string temp_data;
  explicit FHMock(std::string data);
  FHMock();

  std::unique_ptr<I_OStream> get_writer(std::ios::openmode) override;
  std::unique_ptr<I_IStream> get_reader(std::ios::openmode) override;
  bool exists() override;
  std::unique_ptr<I_OStream> get_writer_temp(std::ios::openmode) override;
  void commit_temp_writer() override;
  std::unique_ptr<I_IOStream>
  get_reader_writer(std::ios::openmode openmode) override;
  void clean() override;
};

#endif // RDFCACHEK2_FHMOCK_HPP
