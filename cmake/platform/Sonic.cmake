####
# Sonic.cmake:
#
# Sonic platform file for standard sonic targets.
####
# Set platform default for baremetal scheduler drivers
if (NOT DEFINED FPRIME_USE_BAREMETAL_SCHEDULER)
   set(FPRIME_USE_BAREMETAL_SCHEDULER OFF)
   message(STATUS "Requiring thread library")
   FIND_PACKAGE ( Threads REQUIRED )
endif()
choose_fprime_implementation(Os/File Os/File/Posix)
choose_fprime_implementation(Os/Task Os/Task/Sonic)

# Use common sonic setup
add_definitions(-DTGT_OS_TYPE_LINUX)
set(FPRIME_USE_POSIX ON)

# Add Sonic specific headers into the system
include_directories(SYSTEM "${CMAKE_CURRENT_LIST_DIR}/types")

