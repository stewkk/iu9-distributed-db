#pragma once

#include <format>

#include <boost/outcome.hpp>
#include <boost/outcome/success_failure.hpp>

namespace stewkk::db::logic::result {

struct EmptyResult {};

template <typename T = EmptyResult> using Result = boost::outcome_v2::result<T, std::exception_ptr>;

template <typename T = EmptyResult> auto Ok(T&& v = {}) { return boost::outcome_v2::success(v); }

std::string What(std::exception_ptr ex_ptr);

template <typename... Args>
std::exception_ptr Error(const std::format_string<Args...> fmt, Args&&... args) {
  return std::make_exception_ptr(std::runtime_error{std::format(fmt, std::forward<Args>(args)...)});
}

template <typename T, typename... Args>
std::exception_ptr WrapError(Result<T>&& result, const std::format_string<Args...> fmt,
                             Args&&... args) {
  return Error("{}: {}", std::format(fmt, std::forward<Args>(args)...),
               What(result.assume_error()));
}

}  // namespace stewkk::db::logic::result
