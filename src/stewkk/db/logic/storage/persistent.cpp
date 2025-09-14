#include <stewkk/db/logic/storage/persistent.hpp>

#include <algorithm>
#include <format>

#include <absl/log/log.h>
#include <google/protobuf/util/delimited_message_util.h>

#include <persistent.pb.h>

namespace stewkk::db::logic::storage {

using result::MakeError;
using result::WrapError;

namespace {

static constexpr std::string_view kFailedToLoad = "failed to load persistent storage";
static constexpr std::string_view kFailedToCreate = "failed to create persistent storage";

}  // namespace

Result<PersistentStorage> NewPersistentStorage(std::vector<StorageEntry> data) {
  auto extension = "data";
  auto path = filesystem::GetPath(extension);
  LOG(INFO) << std::format("creating new persistent storage at {}", path.string());
  auto f_opt = filesystem::CreateBinaryFile(path);
  if (f_opt.has_failure()) {
    return WrapError(std::move(f_opt), kFailedToCreate);
  }
  auto f = std::move(f_opt).assume_value();

  std::sort(data.begin(), data.end());

  std::vector<uint64_t> index;
  index.reserve(data.size());
  for (size_t i = 0; i < data.size(); i++) {
    const auto& entry = data[i];
    persistent::Entry protobuf_entry;
    protobuf_entry.set_key(entry.key);
    if (entry.value.has_value()) {
      protobuf_entry.set_value(entry.value.value());
    }

    auto offset = f.tellp();
    index.push_back(offset);

    bool is_ok = google::protobuf::util::SerializeDelimitedToOstream(protobuf_entry, &f);
    if (!is_ok) {
      return MakeError("failed to serialize entry");
    }
  }

  persistent::TableIndex persistent_index;
  persistent_index.set_map_size(data.size());
  for (size_t i = 0; i < data.size(); i++) {
    persistent_index.add_index(index[i]);
  }

  auto index_path = path;
  index_path.replace_extension(".index");
  auto index_f_opt = filesystem::CreateBinaryFile(index_path);
  if (index_f_opt.has_failure()) {
    return WrapError(std::move(index_f_opt), "failed to create index file");
  }
  persistent_index.SerializeToOstream(&index_f_opt.assume_value());

  f.flush();

  auto fd_opt = filesystem::OpenBinaryFD(path);
  if (fd_opt.has_failure()) {
    return WrapError(std::move(fd_opt), "failed to open persistent storage");
  }
  auto fd = std::move(fd_opt).assume_value();

  return PersistentStorage(std::move(path), fd, std::move(index));
}

PersistentStorage::PersistentStorage(fs::path path, int fd, std::vector<uint64_t> index)
    : path_(std::move(path)), fd_(fd), index_(std::move(index)) {}

fs::path PersistentStorage::Path() const { return path_; }

Result<StorageEntry> PersistentStorage::Get(std::string_view key) {
  auto it = std::lower_bound(index_.cbegin(), index_.cend(), key,
                             [this](const int64_t& offset, std::string_view key) -> bool {
                               auto entry_opt = ReadEntryAt(offset);
                               auto [entry_key, _, _] = entry_opt.value();
                               return entry_key < key;
                             });

  if (it != index_.cend()) {
    auto entry = ReadEntryAt(*it).value();
    if (entry.key == key) {
      return entry;
    }
  }
  return MakeError("key {} not found in persistent storage {}", key, path_.string());
}

Result<PersistentStorage> LoadPersistentStorage(fs::path path) {
  LOG(INFO) << std::format("loading persistent storage at {}", path.string());
  auto index_path = path;
  index_path.replace_extension(".index");
  auto index_f_opt = filesystem::OpenBinaryFile(index_path);
  if (index_f_opt.has_failure()) {
    return WrapError(std::move(index_f_opt), "failed to load index file");
  }
  auto index_f = std::move(index_f_opt).assume_value();

  persistent::TableIndex header;
  bool is_ok = header.ParseFromIstream(&index_f);
  if (!is_ok) {
    return MakeError("failed to parse persistent storage header");
  }

  std::vector<uint64_t> index(header.index().cbegin(), header.index().cend());

  auto f_opt = filesystem::OpenBinaryFD(path);
  if (f_opt.has_failure()) {
    return WrapError(std::move(f_opt), "failed to open persistent storage");
  }
  auto fd = std::move(f_opt).assume_value();

  return PersistentStorage(std::move(path), fd, std::move(index));
}

PersistentStorage::~PersistentStorage() {
  if (fd_ != -1) {
    close(fd_);
  }
}

PersistentStorage::PersistentStorage(PersistentStorage&& other)
    : fd_(std::move(other.fd_)), path_(std::move(other.path_)), index_(std::move(other.index_)) {
  other.fd_ = -1;
}

PersistentStorage& PersistentStorage::operator=(PersistentStorage&& other) {
  fd_ = std::move(other.fd_);
  other.fd_ = -1;
  path_ = std::move(other.path_);
  index_ = std::move(other.index_);
  return *this;
}

Result<StorageEntry> PersistentStorage::ReadEntryAt(uint64_t offset) const {
  lseek(fd_, offset, SEEK_SET);
  google::protobuf::io::FileInputStream stream(fd_);
  persistent::Entry entry;
  VLOG(3) << "trying to read entry at offset = " << offset << " storage = " << path_
          << " greatest offset = " << index_.back();
  bool is_ok = google::protobuf::util::ParseDelimitedFromZeroCopyStream(&entry, &stream, nullptr);
  if (!is_ok) {
    return MakeError("failed to read entry from persistent storage");
  }
  StorageEntry res;
  res.key = entry.key();
  if (entry.has_value()) {
    res.value = entry.value();
  }
  return res;
}

PersistentStorage::Iterator PersistentStorage::begin() const {
  return Iterator{this, index_.cbegin()};
}
PersistentStorage::Iterator PersistentStorage::end() const { return Iterator{this, index_.cend()}; }

PersistentStorage::Iterator::value_type PersistentStorage::Iterator::operator*() const {
  auto entry_opt = storage->ReadEntryAt(*index_iterator);
  if (entry_opt.has_failure()) {
    return entry_opt.assume_error();
  }

  return std::move(entry_opt).assume_value();
}

PersistentStorage::Iterator& PersistentStorage::Iterator::operator++() {
  ++index_iterator;
  return *this;
}

PersistentStorage::Iterator PersistentStorage::Iterator::operator++(int) {
  auto tmp = *this;
  ++index_iterator;
  return tmp;
}

}  // namespace stewkk::db::logic::storage
