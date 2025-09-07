#pragma once

#include <atomic>

namespace stewkk::db::logic::synchronization {

// Owning reference counter with spinlock wait implementation.
template <typename T> class OwningReferenceCounter {
public:
  template <typename... Args> OwningReferenceCounter(Args&&... args)
      : ptr_(new T{std::forward<Args>(args)...}), reference_count_(0) {}

  explicit OwningReferenceCounter(T* ptr) : ptr_(ptr), reference_count_(0) {}

  ~OwningReferenceCounter() {
    if (ptr_) {
      Wait();
      delete ptr_;
    }
  }

  OwningReferenceCounter(OwningReferenceCounter&& other)
      : ptr_(std::move(other.ptr_)), reference_count_(0) {
    other.ptr_ = nullptr;
    other.reference_count_.store(0);
  }

  OwningReferenceCounter& operator=(OwningReferenceCounter&& other) {
    ptr_ = std::move(other.ptr_);
    other.ptr = nullptr;
    reference_count_.store(0);
    return *this;
  }

  T* Add() {
    ++reference_count_;
    return ptr_;
  }

  void Remove() { --reference_count_; }

  T* GetUnderlying() { return ptr_; }

  void Wait() {
    while (reference_count_) {
      __builtin_ia32_pause();
    }
  }

private:
  T* ptr_;
  std::atomic<uint32_t> reference_count_;
};

// Non-owning reference counter with spinlock wait implementation.
template <typename T> class ReferenceCounter {
public:
  explicit ReferenceCounter(T* ptr) : ptr_(ptr), reference_count_(0) {}

  ~ReferenceCounter() {
    if (ptr_) {
      Wait();
    }
  }

  ReferenceCounter(ReferenceCounter&& other) : ptr_(std::move(other.ptr_)), reference_count_(0) {
    other.ptr_ = nullptr;
    other.reference_count_.store(0);
  }

  ReferenceCounter& operator=(ReferenceCounter&& other) {
    ptr_ = std::move(other.ptr_);
    other.ptr = nullptr;
    reference_count_.store(0);
    return *this;
  }

  T* Add() {
    ++reference_count_;
    return ptr_;
  }

  void Remove() { --reference_count_; }

  T* GetUnderlying() { return ptr_; }

  void Wait() {
    while (reference_count_) {
      __builtin_ia32_pause();
    }
  }

private:
  T* ptr_;
  std::atomic<uint32_t> reference_count_;
};

}  // namespace stewkk::db::logic::synchronization
