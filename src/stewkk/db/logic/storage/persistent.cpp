#include <stewkk/db/logic/storage/persistent.hpp>

#include <algorithm>
#include <format>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace stewkk::db::logic::storage {

namespace {

static constexpr std::string_view kVersion = "v1";
static constexpr std::string_view kPathFormat = "{}/{}-{}";

static constexpr std::string_view kDir = "/tmp/iu9-distributed-db";

fs::path GetPath() {
  return std::format(kPathFormat, kDir, kVersion,
                     boost::uuids::to_string(boost::uuids::random_generator()()));
}

}  // namespace

Result<PersistentStorage> NewPersistentStorage(std::vector<KwPair> data) {
  auto path = GetPath();
  auto f_opt = CreateBinaryFile(path);
  if (f_opt.has_failure()) {
    return result::WrapError(std::move(f_opt), "failed to create persistent storage");
  }
  auto& f = f_opt.value();

  Metadata meta{.map_size = data.size()};
  f.write(reinterpret_cast<char*>(&meta), sizeof(meta));

  std::sort(data.begin(), data.end());
  IndexBuilder index_builder;
  for (const auto& kw_pair : data) {
    index_builder.AddRecord(kw_pair);
  }
  auto index = index_builder.GetIndex();
  f.write(reinterpret_cast<char*>(index.data()),
          index.size() * sizeof(decltype(index)::value_type));

  for (auto&& el : data) {
    f << std::move(el).key << std::move(el).value;
  }

  auto res_opt = OpenBinaryFile(path);
  if (res_opt.has_failure()) {
    return result::WrapError(std::move(res_opt), "failed to open persistent storage");
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

std::string PersistentStorage::ReadString(PersistentString persistent_str) {
  f_.seekg(persistent_str.index + data_offset_);
  std::string res(persistent_str.size, '\0');
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
    return result::Error("key {} not found in persistent storage {}", key, path_.string());
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

}  // namespace stewkk::db::logic::storage
