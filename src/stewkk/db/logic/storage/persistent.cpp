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

  return PersistentStorage(std::move(path), std::move(index));
}

PersistentStorage::PersistentStorage(fs::path path, std::vector<uint64_t> index)
    : path_(std::move(path)), index_(std::move(index)) {}

fs::path PersistentStorage::Path() const { return path_; }

Result<StorageEntry> PersistentStorage::Get(std::string_view key) {
  auto it = std::lower_bound(index_.cbegin(), index_.cend(), key,
                             [this](const int64_t& offset, std::string_view key) -> bool {
                               auto entry_opt = ReadEntryAt(offset);
                               auto [entry_key, _] = entry_opt.value();
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

  return PersistentStorage(std::move(path), std::move(index));
}

Result<StorageEntry> PersistentStorage::ReadEntryAt(uint64_t offset) {
  auto f_opt = filesystem::OpenBinaryFile(path_);
  if (f_opt.has_failure()) {
    return WrapError(std::move(f_opt), "failed to open persistent storage");
  }
  auto f = std::move(f_opt).assume_value();

  f.seekg(offset);
  persistent::Entry entry;
  google::protobuf::io::IstreamInputStream stream(&f);
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

}  // namespace stewkk::db::logic::storage
