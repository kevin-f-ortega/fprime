####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
add_custom_target("${FPRIME_CURRENT_MODULE}")

#### Shared Section ####
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/error.cpp"
)
set(HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/error.hpp"
)
register_fprime_module(Os_Sonic_Shared)


#### Os/Task/Posix Section ####
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Task.cpp"
)
set(HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Task.hpp"
)

set(MOD_DEPS Os_Sonic_Shared Fw/Time Os)
register_fprime_module(Os_Task_Sonic)
register_fprime_implementation(Os/Task Os_Task_Sonic "${CMAKE_CURRENT_LIST_DIR}/DefaultTask.cpp")
