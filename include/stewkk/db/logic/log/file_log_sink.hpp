#pragma once

#include <fstream>
#include <string_view>

#include <absl/log/log_sink.h>

#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::log {

class FileLogSink : public absl::LogSink {
public:
  static result::Result<std::unique_ptr<FileLogSink>> New(std::string_view file_name);

  FileLogSink(std::ofstream stream);
  void Send(const absl::LogEntry& e) override;
  void Flush() override;

private:
  std::ofstream stream_;
};

}  // namespace stewkk::db::logic::log
