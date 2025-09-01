#pragma once

#include <filesystem>

#include <boost/asio/executor.hpp>

#include <stewkk/db/logic/recovery/swappable_wal_writer_impl.hpp>
#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/logic/storage/memstorage.hpp>
#include <stewkk/db/logic/storage/persistent_collection.hpp>
#include <stewkk/db/logic/storage/swappable_memstorage.hpp>

namespace stewkk::db::logic::recovery {

using result::Result;
namespace fs = std::filesystem;

enum class OperationType { kInsert, kRemove };

std::string ToString(OperationType type);

struct Operation {
  OperationType type;
  std::string key;
  std::optional<std::string> value;

  bool operator<=>(const Operation& other) const = default;
};

Result<std::pair<std::vector<Operation>, int64_t>> ReadWAL(fs::path path);

Result<std::vector<fs::path>> SearchWALFiles();

void Apply(const std::vector<Operation>& operations, storage::KwStorage& storage);

Result<std::tuple<storage::PersistentStorageCollection, storage::SwappableMemoryStorage,
                  SwappableWalWriterImpl>>
InitializeStorages(boost::asio::any_io_executor executor, size_t threshold = 5000);

}  // namespace stewkk::db::logic::recovery
