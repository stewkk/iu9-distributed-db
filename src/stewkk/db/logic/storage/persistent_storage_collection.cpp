#include <stewkk/db/logic/storage/persistent_collection.hpp>

#include <ranges>

#include <absl/log/log.h>
#include <google/protobuf/util/delimited_message_util.h>

#include <persistent.pb.h>

namespace stewkk::db::logic::storage {

namespace {

constexpr static std::string_view kMetadata = "/tmp/iu9-distributed-db/persistent.metadata";

Result<> AppendMetadata(const PersistentStorage& storage) {
  auto got = filesystem::CreateBinaryFile(kMetadata, std::ifstream::app);
  if (got.has_failure()) {
    return result::WrapError(std::move(got), "failed to open metadata file");
  }
  auto& f = got.assume_value();

  persistent::Metadata metadata;
  metadata.set_filename(storage.Path());

  bool is_ok = google::protobuf::util::SerializeDelimitedToOstream(std::move(metadata), &f);
  f.flush();

  if (!is_ok) {
    return result::MakeError("failed to write metadata");
  }
  return result::Ok();
}

std::vector<PersistentStorage> ReadCollection() {
  auto f_opt = filesystem::OpenBinaryFD(kMetadata);
  if (f_opt.has_failure()) {
    LOG(ERROR) << "failed to open metadata file";
    return {};
  }
  auto f = std::move(f_opt).assume_value();

  google::protobuf::io::FileInputStream stream(f);
  std::vector<PersistentStorage> result;

  for (;;) {
    persistent::Metadata metadata;
    bool is_eof = false;
    bool is_ok
        = google::protobuf::util::ParseDelimitedFromZeroCopyStream(&metadata, &stream, &is_eof);
    if (is_eof) {
      break;
    }
    if (!is_ok) {
      LOG(ERROR) << "found bad entry in metadata file";
      return result;
    }

    auto got = LoadPersistentStorage(metadata.filename());
    if (got.has_failure()) {
      LOG(ERROR) << "failed to load metadata file";
      continue;
    }
    result.push_back(std::move(got).assume_value());
  }

  return result;
}

}  // namespace

PersistentStorageCollection::PersistentStorageCollection() : collection_(ReadCollection()) {}

Result<> PersistentStorageCollection::Add(PersistentStorage&& storage) {
  auto got = AppendMetadata(storage);
  if (got.has_failure()) {
    return got;
  }
  collection_.push_back(std::move(storage));
  return result::Ok();
}

Result<KwPair> PersistentStorageCollection::Get(std::string_view key) {
  auto reversed = collection_ | std::ranges::views::reverse;
  for (auto& storage : reversed) {
    auto res = storage.Get(key);
    if (res.has_value()) {
      return res.assume_value();
    }
  }
  return result::MakeError("key {} not found in persistent storage", std::move(key));
}

}  // namespace stewkk::db::logic::storage
