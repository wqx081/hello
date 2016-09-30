#ifndef CORE_BASE_MUTEX_H_
#define CORE_BASE_MUTEX_H_

#include <chrono>
#include <condition_variable>
#include <mutex>

#include "core/base/macros.h"

namespace core {

enum LinkerInitialized { LINKER_INITIALIZED };

class mutex : public std::mutex {
 public:
  mutex() {}
  explicit mutex(LinkerInitialized x) { (void)x; }

  void lock() { std::mutex::lock(); }  
  bool try_lock() { return std::mutex::try_lock(); }
  void unlock() { std::mutex::unlock(); }
};

class mutex_lock : public std::unique_lock<std::mutex> {
 public:
  mutex_lock(class mutex& m) : std::unique_lock<std::mutex>(m) {}
  mutex_lock(class mutex& m, std::try_to_lock_t t) : std::unique_lock<std::mutex>(m,t) {}
  mutex_lock(mutex_lock&& ml) noexcept : std::unique_lock<std::mutex>(std::move(ml)) {}
  ~mutex_lock() {}
};

using std::condition_variable;

enum ConditionResult { kCond_Timeout, kCond_MaybeNotified };

inline ConditionResult WaitForMilliseconds(mutex_lock* mu,
                                           condition_variable* cv,
                                           int64_t ms) {
  std::cv_status s = cv->wait_for(*mu, std::chrono::milliseconds(ms));
  return (s == std::cv_status::timeout) ? kCond_Timeout : kCond_MaybeNotified;
}

} // namespace core
#endif // CORE_BASE_MUTEX_H_
