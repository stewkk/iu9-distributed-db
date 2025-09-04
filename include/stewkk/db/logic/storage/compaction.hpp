#pragma once

#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/storage/persistent_collection.hpp>

namespace stewkk::db::logic::storage {

void RunCompaction(PersistentStorageCollection& collection,
                   const boost::asio::yield_context& yield);

}  // namespace stewkk::db::logic::storage
