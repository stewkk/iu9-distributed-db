#pragma once

#include <format>

#include <boost/outcome.hpp>
#include <boost/outcome/success_failure.hpp>

namespace stewkk::db::logic::result {

struct EmptyResult {};

template <typename T = EmptyResult> using Result = boost::outcome_v2::result<T, std::exception_ptr>;

template <typename T = EmptyResult> auto Ok(T&& v = {}) { return boost::outcome_v2::success(v); }

template <typename... Args>
std::exception_ptr Error(std::format_string<Args...> fmt, Args&&... args) {
  return std::make_exception_ptr(std::runtime_error{std::format(fmt, args...)});
}

}  // namespace stewkk::db::logic::result
