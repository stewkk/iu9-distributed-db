#include <stewkk/db/logic/controllers/controller.hpp>

#include <absl/log/log.h>

#include <stewkk/db/logic/storage/compaction.hpp>

namespace stewkk::db::logic::controllers {

namespace {

constexpr static size_t kThreshold = 5;

}  // namespace

Controller::Controller(storage::SwappableMemoryStorage& storage, recovery::WALWriter& wal_writer,
                       recovery::SwappableWalWriter& swappablewal_writer,
                       storage::PersistentStorageCollection& persistent_storage,
                       coordination::VersionNumberGenerator& version_generator,
                       boost::asio::any_io_executor executor)
    : storage_(storage),
      wal_writer_(wal_writer),
      swappable_wal_writer_(swappablewal_writer),
      persistent_storage_(persistent_storage),
      swap_job_guard_(),
      version_generator_(version_generator),
      executor_(executor) {}

void Controller::TriggerSwapToPersistentStorage() {
  if (storage_.Size() < kThreshold) {
    return;
  }
  if (!swap_job_guard_.TryAcquire()) {
    return;
  }
  boost::asio::spawn(executor_, [&](boost::asio::yield_context yield) {
    SwapToPersistentStorage(yield);
    swap_job_guard_.Release();
  });
}

void Controller::SwapToPersistentStorage(const boost::asio::yield_context& yield) {
  auto remove_callback_opt = swappable_wal_writer_.Swap(yield);
  if (remove_callback_opt.has_failure()) {
    LOG(ERROR) << "SwapToPersistentStorage failed: " << remove_callback_opt.assume_error().What();
    return;
  }
  auto remove_callback = std::move(remove_callback_opt).assume_value();

  auto data = storage_.Collect();

  auto persistent_opt = storage::NewPersistentStorage(std::move(data));
  if (persistent_opt.has_failure()) {
    LOG(ERROR) << "SwapToPersistentStorage failed: " << persistent_opt.assume_error().What();
    return;
  }

  auto err = persistent_storage_.Add(std::move(persistent_opt).assume_value(), yield);
  if (err.has_failure()) {
    LOG(ERROR) << "SwapToPersistentStorage failed: " << err.assume_error().What();
    return;
  }

  storage_.RemoveCollected();

  remove_callback();

  storage::RunCompaction(persistent_storage_, yield);
}

}  // namespace stewkk::db::logic::controllers
