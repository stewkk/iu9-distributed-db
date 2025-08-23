#pragma once

#include <exception>

namespace stewkk::db::views {

struct RethrowFirstArg {
  template <class... T> void operator()(std::exception_ptr ep, T&&...) {
    if (ep) {
      std::rethrow_exception(ep);
    }
  }

  template <class... T> void operator()(T&&...) {}
};

}  // namespace stewkk::db::views
