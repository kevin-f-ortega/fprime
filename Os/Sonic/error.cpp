// ======================================================================
// \title Os/Sonic/error.cpp
// \brief implementation for sonic errno conversion
// ======================================================================
#include "Os/Sonic/error.hpp"
#include <cerrno>

namespace Os {
namespace Sonic {

Task::Status sonic_status_to_task_status(PlatformIntType sonic_status) {
    Task::Status status = Task::Status::OP_OK;
    switch (sonic_status) {
        case 0:
            status = Task::Status::OP_OK;
            break;
        case EINVAL:
            status = Task::Status::INVALID_PARAMS;
            break;
        case EPERM:
            status = Task::Status::ERROR_PERMISSION;
            break;
        case EAGAIN:
            status = Task::Status::ERROR_RESOURCES;
            break;
        default:
            status = Task::Status::UNKNOWN_ERROR;
            break;
    }
    return status;
}

}  // namespace Sonic
}  // namespace Os
