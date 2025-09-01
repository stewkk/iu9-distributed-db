#pragma once

#include <atomic>

namespace stewkk::db::logic::synchronization {

class JobGuard {
public:
  JobGuard();
  JobGuard(const JobGuard& other);
  JobGuard& operator=(const JobGuard& other);

  bool TryAcquire();
  void Release();

private:
  std::atomic_flag is_job_running;
};

}  // namespace stewkk::db::logic::synchronization
