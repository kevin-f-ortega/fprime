# How to add a new OSAL

Let's Sonic be the name of the new OSAL you want to create

1. `cd fprime/Os`
2. `mkdir Sonic`
3. `vi CMakeLists.txt`
    ```cmake
    add_fprime_subdirectory(“${CMAKE_CURRENT_LIST_DIR}/Sonic”)
    ```

4. `vi Sonic/CMakeLists.txt`
    ```cmake
    ####
    # F prime CMakeLists.txt:
    #
    # SOURCE_FILES: combined list of source and autocoding files
    # MOD_DEPS: (optional) module dependencies
    #
    ####
    add_custom_target("${FPRIME_CURRENT_MODULE}")
    
    #### Os/Task/Posix Section ####
    set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Task.cpp"
    )
    set(HEADER_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Task.hpp"
    )
    
    set(MOD_DEPS Fw/Time Os)
    register_fprime_module(Os_Task_Sonic)
    register_fprime_implementation(Os/Task Os_Task_Sonic "${CMAKE_CURRENT_LIST_DIR}/DefaultTask.cpp")
    ```

5. `touch Os/Sonic/DefaultTask.cpp`
6. Implement the two static functions 1) delay and 2) getDelegate in this file. See `Os/Posix/DefaultTask.cpp` for example.
7. Implement SonicTask, which will inherit from TaskInterface.
    1. Add `Os/Sonic/Task.hpp` and `Os/Sonic/Task.cpp` 
8. Add platform file:
    1. `vi fprime/cmake/platform/Sonic.cmake`
    ```cmake
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
    ```

9. Add toolchain file:
    1. `vi fprime/cmake/toolchain/sonic.cmake`

    ```cmake
    ## STEP 2: Specify the target system's name. i.e. raspberry-pi-3
    set(CMAKE_SYSTEM_NAME "Sonic")
    
    # STEP 3: Specify the path to C and CXX cross compilers
    set(CMAKE_C_COMPILER "/usr/bin/gcc")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
    
    # STEP 4: Specify paths to root of toolchain package, for searching for
    #         libraries, executables, etc.
    set(CMAKE_FIND_ROOT_PATH  "/usr")
    
    # DO NOT EDIT: F prime searches the host for programs, not the cross
    # compile toolchain
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    # DO NOT EDIT: F prime searches for libs, includes, and packages in the
    # toolchain when cross-compiling.
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
    ```

10. Update fprime/Drv/CMakeLists.txt line 19 to this
    ```cmake
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" OR ${CMAKE_SYSTEM_NAME} STREQUAL "VxWorks" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Sonic")
    ```
11. Update fprime/Svc/CMakeLists.txt line 56 to this
    ```cmake
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Sonic")
    ```
12. Update fprime/Os/CMakeLists.txt line 75 to
    ```cmake
    elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Sonic")
    ```