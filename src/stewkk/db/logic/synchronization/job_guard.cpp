#include <stewkk/db/logic/synchronization/job_guard.hpp>

namespace stewkk::db::logic::synchronization {

JobGuard::JobGuard() : is_job_running(false) {}

JobGuard::JobGuard(const JobGuard& other) : is_job_running(false) {}

JobGuard& JobGuard::operator=(const JobGuard& other) {
  is_job_running.clear();
  return *this;
}

bool JobGuard::TryAcquire() { return !is_job_running.test_and_set(); }

void JobGuard::Release() { is_job_running.clear(); }

}  // namespace stewkk::db::logic::synchronization
