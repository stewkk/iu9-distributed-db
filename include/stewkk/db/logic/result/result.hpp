#pragma once

#include <boost/outcome.hpp>
#include <boost/outcome/success_failure.hpp>

namespace stewkk::db::logic::result {

struct EmptyResult {};

template <typename T = EmptyResult> using Result = boost::outcome_v2::result<T, std::exception_ptr>;

template <typename T = EmptyResult> auto Ok(T&& v) { return boost::outcome_v2::success(v); }

std::exception_ptr Error(const std::exception& e);

}  // namespace stewkk::db::logic::result
