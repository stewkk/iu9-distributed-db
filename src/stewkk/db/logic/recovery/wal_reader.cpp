#include <stewkk/db/logic/recovery/wal_reader.hpp>

#include <absl/log/log.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/util/delimited_message_util.h>

#include <wal.pb.h>

#include <stewkk/db/logic/filesystem/filesystem.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

namespace stewkk::db::logic::recovery {

Result<std::pair<std::vector<Operation>, int64_t>> ReadWAL(fs::path path) {
  LOG(INFO) << std::format("loading WAL file: {}", path.string());
  auto f_opt = filesystem::OpenBinaryFD(path);
  if (f_opt.has_failure()) {
    return result::WrapError(std::move(f_opt), "failed to open WAL file");
  }
  auto f = std::move(f_opt).assume_value();

  google::protobuf::io::FileInputStream stream(f);
  std::vector<Operation> result;

  int64_t last_valid = 0;
  for (;;) {
    last_valid = stream.ByteCount();
    wal::Entry entry;
    bool is_eof = false;
    bool is_ok = google::protobuf::util::ParseDelimitedFromZeroCopyStream(&entry, &stream, &is_eof);
    if (is_eof) {
      break;
    }
    if (!is_ok) {
      LOG(ERROR) << std::format("found bad entry in WAL file {} at {}", path.string(), last_valid);
      return result::Ok(std::make_pair(std::move(result), last_valid));
    }

    // NOTE: better to move into separate function
    if (entry.has_insert()) {
      result.push_back(Operation{
          OperationType::kInsert,
          entry.insert().key(),
          entry.insert().value(),
      });
    } else if (entry.has_remove()) {
      result.push_back(Operation{
          OperationType::kRemove,
          entry.remove().key(),
          std::nullopt,
      });
    }
  }

  return result::Ok(std::make_pair(std::move(result), last_valid));
}

std::string ToString(OperationType type) {
  switch (type) {
    case OperationType::kInsert:
      return "insert";
    case OperationType::kRemove:
      return "remove";
  }
}

Result<std::vector<fs::path>> SearchWALFiles() {
  std::vector<fs::path> result;
  result.reserve(2);
  auto dir = filesystem::GetDataDir();
  try {
    for (const auto& entry : std::filesystem::directory_iterator{dir}) {
      if (entry.path().extension() == ".wal") {
        result.push_back(entry.path());
      }
    }
  } catch (const std::exception& ex) {
    return result::MakeError("failed to search for .wal files: {}", ex.what());
  }
  return result;
}

void Apply(const std::vector<Operation>& operations, storage::KwStorage& storage) {
  for (auto& operation : operations) {
    switch (operation.type) {
      case OperationType::kInsert:
        storage.Insert(models::storage::KwPair{operation.key, operation.value.value()});
        break;
      case OperationType::kRemove:
        storage.Remove(operation.key);
        break;
    }
  }
}

Result<std::pair<storage::PersistentStorageCollection, storage::SwappableMemoryStorage>>
InitializeStorages(size_t threshold) {
  storage::PersistentStorageCollection persistent_collection;
  storage::SwappableMemoryStorage memstorage;
  auto files = SearchWALFiles().value();
  if (files.size() > 2) {
    LOG(WARNING) << "more than 2 WAL files";
  }
  for (const auto& file : files) {
    auto [operations, position] = ReadWAL(file).value();
    storage::MapStorage storage;
    Apply(operations, storage);
    if (storage.Size() > threshold) {
      auto persistent_opt
          = storage::NewPersistentStorage(storage::ReadonlyMemoryStorage(std::move(storage)));
      if (persistent_opt.has_failure()) {
        return result::WrapError(std::move(persistent_opt),
                                 "failed to initialize persistent storage");
      }
      auto got = persistent_collection.Add(std::move(persistent_opt).assume_value());
      if (got.has_failure()) {
        return result::WrapError(std::move(got), "failed to initialize persistent collection");
      }
    } else {
      storage::MapStorage* ptr = new storage::MapStorage(std::move(storage));
      memstorage = storage::SwappableMemoryStorage{ptr};
    }
  }
  return {std::move(persistent_collection), std::move(memstorage)};
}

}  // namespace stewkk::db::logic::recovery
