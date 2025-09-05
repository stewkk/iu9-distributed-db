#include <stewkk/db/logic/log/file_log_sink.hpp>

#include <absl/log/log.h>

namespace stewkk::db::logic::log {

namespace {

std::string_view GetLogSeverityPrefix(absl::LogSeverity severity) {
  switch (severity) {
    case absl::LogSeverity::kInfo:
      return "[I]";
    case absl::LogSeverity::kError:
      return "[E]";
    case absl::LogSeverity::kWarning:
      return "[W]";
    case absl::LogSeverity::kFatal:
      return "[F]";
  }
}

}  // namespace

result::Result<std::unique_ptr<FileLogSink>> FileLogSink::New(std::string_view file_name) {
  std::ofstream s(std::string(file_name).c_str(), std::ofstream::app);
  if (!s) {
    return result::MakeError("could not open file: {}", file_name);
  }
  return std::make_unique<FileLogSink>(std::move(s));
}

void FileLogSink::Send(const absl::LogEntry& e) {
  stream_ << GetLogSeverityPrefix(e.log_severity()) << "\t" << e.text_message_with_newline();
  if (e.log_severity() == absl::LogSeverity::kFatal) {
    Flush();
  }
};

void FileLogSink::Flush() { stream_.flush(); };

FileLogSink::FileLogSink(std::ofstream stream) { stream_ = std::move(stream); }

}  // namespace stewkk::db::logic::log
