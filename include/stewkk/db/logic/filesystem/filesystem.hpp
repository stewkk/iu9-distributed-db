#pragma once

#include <filesystem>
#include <fstream>
#include <vector>

#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::filesystem {

using result::MakeError;
using result::Ok;
using result::Result;
namespace fs = std::filesystem;

static constexpr std::string_view kFailedToRead = "failed to read file: {}";
static constexpr std::string_view kFailedToWrite = "failed to write file: {}";

fs::path GetPath(std::string_view extension);

fs::path GetDataDir();

Result<std::ofstream> CreateBinaryFile(fs::path path);

Result<std::ifstream> OpenBinaryFile(fs::path path);

Result<std::int32_t> OpenBinaryFD(fs::path path);

template <typename> struct type {};

template <typename T> Result<T> ReadFromFile(type<T>, std::ifstream& f) {
  T res;
  try {
    f.read(reinterpret_cast<char*>(&res), sizeof(T));
  } catch (const std::exception& ex) {
    return MakeError(kFailedToRead, ex.what());
  }
  return res;
}

template <typename T>
Result<std::vector<T>> ReadFromFile(type<std::vector<T>>, std::ifstream& f, std::size_t size) {
  std::vector<T> res(size);
  try {
    f.read(reinterpret_cast<char*>(res.data()), res.size() * sizeof(T));
  } catch (const std::exception& ex) {
    return MakeError(kFailedToRead, ex.what());
  }
  return res;
}

template <typename T> Result<T> ReadFromFile(std::ifstream& f) {
  return ReadFromFile(type<T>{}, f);
}

template <typename T> Result<T> ReadFromFile(std::ifstream& f, std::size_t size) {
  return ReadFromFile(type<T>{}, f, size);
}

template <typename T> Result<> WriteToFile(std::ofstream& f, const T& data) {
  try {
    f.write(reinterpret_cast<const char*>(&data), sizeof(data));
    return Ok();
  } catch (const std::exception& ex) {
    return MakeError(kFailedToWrite, ex.what());
  }
}

template <typename T> Result<> WriteToFile(std::ofstream& f, const std::vector<T>& vec) {
  try {
    f.write(reinterpret_cast<const char*>(vec.data()), vec.size() * sizeof(T));
    return Ok();
  } catch (const std::exception& ex) {
    return MakeError(kFailedToWrite, ex.what());
  }
}

}  // namespace stewkk::db::logic::filesystem
