#include <stewkk/db/logic/storage/persistent_collection.hpp>

#include <ranges>

#include <absl/log/log.h>
#include <boost/scope_exit.hpp>

#include <persistent.pb.h>

namespace stewkk::db::logic::storage {

namespace {

constexpr static std::string_view kMetadata = "/tmp/iu9-distributed-db/persistent.metadata";

Result<persistent::Metadata> ReadMetadata(fs::path path = kMetadata) {
  auto f_opt = filesystem::OpenBinaryFile(path);
  if (f_opt.has_failure()) {
    return result::MakeError("failed to open metadata file");
  }
  auto& f = f_opt.assume_value();

  persistent::Metadata metadata;
  bool is_ok = metadata.ParseFromIstream(&f);
  if (!is_ok) {
    return result::MakeError("failed to parse metadata file");
  }

  return metadata;
}

Result<> AppendMetadata(const PersistentStorage& storage, fs::path path = kMetadata) {
  persistent::Metadata metadata{};

  auto metadata_opt = ReadMetadata();
  if (metadata_opt.has_value()) {
    metadata = metadata_opt.assume_value();
  }

  auto f_opt = filesystem::CreateBinaryFile(path, std::ofstream::trunc);
  if (f_opt.has_failure()) {
    return result::WrapError(std::move(f_opt), "failed to open metadata file");
  }
  auto& f = f_opt.assume_value();

  auto entry = metadata.add_entry();
  entry->set_filename(storage.Path().filename());
  bool is_ok = metadata.SerializeToOstream(&f);
  f.flush();

  if (!is_ok) {
    return result::MakeError("failed to write metadata");
  }
  return result::Ok();
}

std::vector<PersistentStorage> ReadCollection() {
  std::vector<PersistentStorage> result;

  auto metadata_opt = ReadMetadata();
  if (metadata_opt.has_failure()) {
    LOG(ERROR) << "failed to read metadata file";
    return result;
  }
  auto& metadata = metadata_opt.assume_value();

  for (int i = 0; i < metadata.entry_size(); i++) {
    const auto& entry = metadata.entry(i);

    auto got = LoadPersistentStorage(filesystem::GetDataDir() / entry.filename());
    if (got.has_failure()) {
      LOG(ERROR) << "failed to load metadata file";
      continue;
    }
    result.push_back(std::move(got).assume_value());
  }

  return result;
}

}  // namespace

PersistentStorageCollection::PersistentStorageCollection(boost::asio::any_io_executor executor)
    : collection_(ReadCollection()), strand_(executor), executor_(std::move(executor)) {}

Result<> PersistentStorageCollection::Add(PersistentStorage&& storage) {
  auto got = AppendMetadata(storage);
  if (got.has_failure()) {
    return got;
  }
  collection_.push_back(std::move(storage));
  return result::Ok();
}

Result<StorageEntry> PersistentStorageCollection::Get(std::string_view key,
                                                      const boost::asio::yield_context& yield) {
  boost::asio::post(boost::asio::bind_executor(strand_, yield));
  BOOST_SCOPE_EXIT(&yield, this_) {
    boost::asio::post(boost::asio::bind_executor(this_->executor_, yield));
  }
  BOOST_SCOPE_EXIT_END

  auto reversed = collection_ | std::ranges::views::reverse;
  for (auto& storage : reversed) {
    auto res = storage.Get(key);
    if (res.has_value()) {
      return res.assume_value();
    }
  }
  return result::MakeError("key {} not found in persistent storage", std::move(key));
}

size_t PersistentStorageCollection::Size() const { return collection_.size(); }

const std::vector<PersistentStorage>& PersistentStorageCollection::GetCollection() const {
  return collection_;
}

Result<> PersistentStorageCollection::SwapWith(PersistentStorage&& storage,
                                               const boost::asio::yield_context& yield) {
  fs::path new_path = kMetadata;
  new_path.concat(".new");

  std::error_code ec;
  fs::remove(new_path, ec);

  auto err = AppendMetadata(storage, new_path);
  if (err.has_failure()) {
    return err;
  }

  fs::rename(new_path, kMetadata);

  std::vector<fs::path> to_delete;
  to_delete.reserve(collection_.size());
  for (const auto& storage : collection_) {
    to_delete.push_back(storage.Path());
  }

  boost::asio::post(boost::asio::bind_executor(strand_, yield));
  std::vector<PersistentStorage> new_collection;
  new_collection.emplace_back(std::move(storage));
  collection_ = std::move(new_collection);
  boost::asio::post(boost::asio::bind_executor(executor_, yield));

  for (const auto& path : to_delete) {
    fs::remove(path);
  }

  return result::Ok();
}

}  // namespace stewkk::db::logic::storage
