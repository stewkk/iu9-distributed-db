#pragma once

#include <filesystem>
#include <fstream>
#include <vector>

#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::storage {

using result::Error;
using result::Result;
namespace fs = std::filesystem;

Result<std::ofstream> CreateBinaryFile(fs::path path);

Result<std::ifstream> OpenBinaryFile(fs::path path);

template <typename> struct type {};

template <typename T> Result<T> ReadFromFile(type<T>, std::ifstream& f) {
  T res;
  try {
    f.read(reinterpret_cast<char*>(&res), sizeof(T));
  } catch (const std::exception& ex) {
    return Error("failed to read file: {}", ex.what());
  }
  return res;
}

template <typename T>
Result<std::vector<T>> ReadFromFile(type<std::vector<T>>, std::ifstream& f, std::size_t size) {
  std::vector<T> res(size);
  try {
    f.read(reinterpret_cast<char*>(res.data()), res.size() * sizeof(T));
  } catch (const std::exception& ex) {
    return Error("failed to read file: {}", ex.what());
  }
  return res;
}

template <typename T> Result<T> ReadFromFile(std::ifstream& f) {
  return ReadFromFile(type<T>{}, f);
}

template <typename T> Result<T> ReadFromFile(std::ifstream& f, std::size_t size) {
  return ReadFromFile(type<T>{}, f, size);
}

}  // namespace stewkk::db::logic::storage
