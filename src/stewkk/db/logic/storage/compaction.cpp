#include <stewkk/db/logic/storage/compaction.hpp>

#include <queue>

#include <absl/log/log.h>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/spawn.hpp>

namespace stewkk::db::logic::storage {

namespace {

constexpr static auto kThreshold = 2;

Result<PersistentStorage> MergeData(PersistentStorageCollection& collection) {
  const auto& storages = collection.GetCollection();
  assert(storages.size() > 0);

  using StorageIterator = decltype(storages.front().begin());
  using IteratorPair = std::pair<StorageIterator, StorageIterator>;
  std::vector<IteratorPair> iterators;
  iterators.reserve(storages.size());
  for (const auto& storage : storages) {
    iterators.emplace_back(storage.begin(), storage.end());
  }

  std::vector<StorageEntry> data;
  // TODO: data.reserve(storages.size()*storages.front().Size());

  auto comparator = [](const IteratorPair& lhs, const IteratorPair& rhs) {
    auto lhs_opt = *(lhs.first);
    auto rhs_opt = *(rhs.first);
    VLOG(3) << "merging lhs value from: " << lhs.first.storage->Path() << " with index "
            << lhs.first.index_iterator - lhs.first.storage->begin().index_iterator << " and size "
            << lhs.first.storage->end().index_iterator - lhs.first.storage->begin().index_iterator;
    VLOG(3) << "merging rhs value from: " << rhs.first.storage->Path() << " with index "
            << rhs.first.index_iterator - rhs.first.storage->begin().index_iterator << " and size "
            << rhs.first.storage->end().index_iterator - rhs.first.storage->begin().index_iterator;
    if (lhs_opt.has_failure()) {
      LOG(ERROR) << "error while merging data from persistent storages: "
                 << lhs_opt.assume_error().What();
      return false;
    }
    if (rhs_opt.has_failure()) {
      LOG(ERROR) << "error while merging data from persistent storages: "
                 << rhs_opt.assume_error().What();
      return false;
    }
    auto lhs_value = std::move(lhs_opt).assume_value();
    auto rhs_value = std::move(rhs_opt).assume_value();
    return lhs_value < rhs_value;
  };

  std::priority_queue<IteratorPair, decltype(iterators), decltype(comparator)> queue(
      iterators.begin(), iterators.end(), comparator);
  while (!queue.empty()) {
    auto [cur, end] = queue.top();
    queue.pop();
    auto data_opt = *cur;
    if (data_opt.has_failure()) {
      return result::WrapError(std::move(data_opt),
                               "failed to merge data from persistent storages");
    }
    data.push_back(std::move(data_opt).assume_value());
    ++cur;
    if (cur != end) {
      queue.push({cur, end});
    }
  }

  return NewPersistentStorage(std::move(data));
}

}  // namespace

void RunCompaction(PersistentStorageCollection& collection,
                   const boost::asio::yield_context& yield) {
  if (collection.Size() <= kThreshold) {
    return;
  }

  LOG(INFO) << "run compaction job";

  auto merge_result = MergeData(collection);
  if (merge_result.has_failure()) {
    LOG(ERROR) << "failed to merge PersistentStorageCollection: "
               << merge_result.assume_error().What();
    return;
  }
  auto new_storage = std::move(merge_result).assume_value();

  auto err = collection.SwapWith(std::move(new_storage), yield);
  if (err.has_failure()) {
    LOG(ERROR) << "failed to swap persistent storages: " << err.assume_error().What();
  }
  LOG(INFO) << "compaction job done";
}

}  // namespace stewkk::db::logic::storage
