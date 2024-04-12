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

#### Os/Task/Sonic Unit Tests ####
set(UT_SOURCE_FILES
        "${FPRIME_FRAMEWORK_PATH}/Os/test/ut/task/CommonTests.cpp"
        "${FPRIME_FRAMEWORK_PATH}/Os/test/ut/task/TaskRules.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/SonicTaskTests.cpp"
)
set(UT_MOD_DEPS
        Os
        STest
)
choose_fprime_implementation(Os/Task Os_Task_Sonic)
register_fprime_ut(SonicTaskTest)