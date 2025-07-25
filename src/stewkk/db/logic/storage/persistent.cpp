#include <stewkk/db/logic/storage/persistent.hpp>

#include <algorithm>
#include <format>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace stewkk::db::logic::storage {

using result::Error;
using result::WrapError;

namespace {

static constexpr std::string_view kVersion = "v1";
static constexpr std::string_view kPathFormat = "{}/{}-{}";

static constexpr std::string_view kDir = "/tmp/iu9-distributed-db";

static constexpr std::string_view kFailedToLoad = "failed to load persistent storage";
static constexpr std::string_view kFailedToCreate = "failed to create persistent storage";

fs::path GetPath() {
  return std::format(kPathFormat, kDir, kVersion,
                     boost::uuids::to_string(boost::uuids::random_generator()()));
}

}  // namespace

Result<PersistentStorage> NewPersistentStorage(std::vector<KwPair> data) {
  auto path = GetPath();
  auto f_opt = CreateBinaryFile(path);
  if (f_opt.has_failure()) {
    return WrapError(std::move(f_opt), kFailedToCreate);
  }
  auto f = std::move(f_opt).assume_value();

  Metadata meta{.map_size = data.size()};
  if (auto res = WriteToFile(f, meta); res.has_failure()) {
    return WrapError(std::move(res), kFailedToCreate);
  }

  std::sort(data.begin(), data.end());
  IndexBuilder index_builder;
  for (const auto& kw_pair : data) {
    index_builder.AddRecord(kw_pair);
  }
  auto index = index_builder.GetIndex();
  if (auto res = WriteToFile(f, index); res.has_failure()) {
    return WrapError(std::move(res), kFailedToCreate);
  }

  for (auto&& el : data) {
    f << std::move(el).key << std::move(el).value;
  }

  auto res_opt = OpenBinaryFile(path);
  if (res_opt.has_failure()) {
    return WrapError(std::move(res_opt), kFailedToCreate);
  }

  return PersistentStorage(std::move(path), std::move(res_opt.value()), std::move(meta),
                           std::move(index));
}

PersistentStorage::PersistentStorage(fs::path path, std::ifstream fstream, Metadata metadata,
                                     std::vector<PersistentKwPair> index)
    : path_(std::move(path)),
      f_(std::move(fstream)),
      metadata_(std::move(metadata)),
      index_(std::move(index)),
      data_offset_(0) {
  data_offset_ = sizeof(Metadata) + index_.size() * sizeof(PersistentKwPair);
}

fs::path PersistentStorage::Path() const { return path_; }

std::string PersistentStorage::ReadString(PersistentString persistent_str) {
  f_.seekg(persistent_str.index + data_offset_);
  std::string res(persistent_str.size, '\0');
  // TODO: turn exception into Result
  f_.read(res.data(), persistent_str.size);
  return res;
}

Result<KwPair> PersistentStorage::Get(std::string_view key) {
  auto it = std::lower_bound(index_.cbegin(), index_.cend(), key,
                             [this](const PersistentKwPair& lhs, std::string_view rhs) -> bool {
                               auto persistent_key = lhs.key;
                               auto key = ReadString(persistent_key);
                               return key < rhs;
                             });

  if (it == index_.cend() || key != ReadString(it->key)) {
    return Error("key {} not found in persistent storage {}", key, path_.string());
  }

  return KwPair{
      .key = ReadString(it->key),
      .value = ReadString(it->value),
  };
}

IndexBuilder::IndexBuilder() : offset_(0) {}

PersistentString IndexBuilder::GetPersistentString(const std::string& str) {
  PersistentString p_str{.index = offset_, .size = str.size()};
  offset_ += str.size();
  return p_str;
}

void IndexBuilder::AddRecord(const KwPair& pair) {
  auto key = GetPersistentString(pair.key);
  auto value = GetPersistentString(pair.value);
  PersistentKwPair p_pair{.key = std::move(key), .value = std::move(value)};
  index_.push_back(std::move(p_pair));
}

std::vector<PersistentKwPair> IndexBuilder::GetIndex() const { return index_; }

Result<PersistentStorage> LoadPersistentStorage(fs::path path) {
  auto f_opt = OpenBinaryFile(path);
  if (f_opt.has_failure()) {
    return WrapError(std::move(f_opt), kFailedToLoad);
  }
  auto f = std::move(f_opt).assume_value();

  auto meta_opt = ReadFromFile<Metadata>(f);
  if (meta_opt.has_failure()) {
    return WrapError(std::move(meta_opt), kFailedToLoad);
  }
  auto meta = std::move(meta_opt).assume_value();

  auto index_opt = ReadFromFile<Index>(f, meta.map_size);
  if (index_opt.has_failure()) {
    return WrapError(std::move(index_opt), kFailedToLoad);
  }
  auto index = std::move(index_opt).assume_value();

  return PersistentStorage(std::move(path), std::move(f), std::move(meta), std::move(index));
}

}  // namespace stewkk::db::logic::storage
