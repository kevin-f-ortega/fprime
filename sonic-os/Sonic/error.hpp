// ======================================================================
// \title Os/Sonic/error.hpp
// \brief header for sonic errno conversion
// ======================================================================
#include "Os/Task.hpp"
#ifndef OS_SONIC_ERRNO_HPP
#define OS_SONIC_ERRNO_HPP

namespace Os {
namespace Sonic {

//! Convert an sonic task representation of an error to the Os::Task::Status representation.
//! \param sonic_status: errno representation of the error
//! \return: Os::Task::Status representation of the error
//!
Os::Task::Status sonic_status_to_task_status(PlatformIntType sonic_status);

}  // namespace Sonic
}  // namespace Os
#endif
