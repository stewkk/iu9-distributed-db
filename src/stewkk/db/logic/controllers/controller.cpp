#include <stewkk/db/logic/controllers/controller.hpp>

namespace stewkk::db::logic::controllers {

Controller::Controller(storage::KwStorage& storage, recovery::WALWriter& wal_writer,
                       storage::PersistentStorageCollection& persistent_storage)
    : storage_(storage), wal_writer_(wal_writer), persistent_storage_(persistent_storage) {}

}  // namespace stewkk::db::logic::controllers
