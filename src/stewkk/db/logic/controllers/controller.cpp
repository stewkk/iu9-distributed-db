#include <stewkk/db/logic/controllers/controller.hpp>

namespace stewkk::db::logic::controllers {

Controller::Controller(storage::KwStorage& storage, recovery::WALWriter& wal_writer)
    : storage_(storage), wal_writer_(wal_writer) {}

}  // namespace stewkk::db::logic::controllers
