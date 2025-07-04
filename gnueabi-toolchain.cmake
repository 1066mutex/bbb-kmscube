
set(SDK_INSTALL_DIR "/opt/arago-2023.04")
set(HOST_SYSROOT    "${SDK_INSTALL_DIR}/sysroots/x86_64-arago-linux")
set(TARGET_SYSROOT  "${SDK_INSTALL_DIR}/sysroots/armv7at2hf-neon-oe-linux-gnueabi")
set(HOST_BIN        "${HOST_SYSROOT}/usr/bin")
set(TOOLCHAIN_NAME  "arm-oe-linux-gnueabi")
set(GCC_DIR         "${HOST_BIN}/${TOOLCHAIN_NAME}")

set(CMAKE_SYSTEM_NAME       Linux)
set(CMAKE_SYSTEM_PROCESSOR "arm")

set(CMAKE_C_COMPILER    "${GCC_DIR}/${TOOLCHAIN_NAME}-gcc")
set(CMAKE_CXX_COMPILER  "${GCC_DIR}/${TOOLCHAIN_NAME}-g++")

set(ARCH_FLAGS          " -march=armv7-a -mthumb -mfpu=neon -mfloat-abi=hard")
set(CMAKE_C_FLAGS       "${ARCH_FLAGS} -lm -lc " CACHE STRING "" FORCE )
set(CMAKE_CXX_FLAGS     "${ARCH_FLAGS}" CACHE STRING "" FORCE )
set(CMAKE_ASM_FLAGS     ${CMAKE_C_FLAGS} CACHE STRING "" FORCE )
set(CMAKE_LDFLAGS_FLAGS ${CMAKE_C_FLAGS} CACHE STRING "" FORCE )

set(CMAKE_SYSROOT "${TARGET_SYSROOT}")

set(CMAKE_FIND_ROOT_PATH "${TARGET_SYSROOT}" )

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY )

set(CMAKE_FIND_LIBRARY_CUSTOM_LIB_SUFFIX "")

set(CMAKE_PROGRAM_PATH "${HOST_SYSROOT}/usr/bin" CACHE STRING "" FORCE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# option to turn QSPY software tracing ON or OFF
# NOTE: change of the option might require re-building the project
# USAGE: 
#
option(QSPY "QSPY software tracing" OFF)

# option for verbose 
option(VERBOSE_ "print out debug info" OFF)

# option for simulate 
option(SIMULATE_ "simulating HW " OFF)

# option for host build 
option(HOST_BUILD_ "host build " OFF)


set(BOARD_IP 192.168.0.130)
set(PORT_NO 3000)
set(TARGET_DIR /usr/bin/)
set(TARGET_BINARY ${PROJECT_NAME})

# create targets that will execute custom commands
add_custom_target(build ALL)
add_custom_target(host_build ALL DEPENDS ${TARGET_BINARY}) 
add_custom_target(build_debug ALL DEPENDS ${TARGET_BINARY}) 
add_custom_target(build_deploy ALL DEPENDS ${TARGET_BINARY})
add_custom_target(qspy_debug ALL DEPENDS ${TARGET_BINARY})
#add_custom_target(run_test COMMAND make
#                 ${CMAKE_CURRENT_LIST_DIR}/tests/test_packer)
add_custom_target(unit_test ALL)

message(STATUS "Deploy executable ${PROJECT_NAME} to remote target ${BOARD_IP}:${PORT_NO} ${TARGET_DIR} ") 

# specify commands that have to be executed to generate files
# or at specific build events of the above targets

# copy the binary to the target and start gdb and binary file.
# !!! these commands are added to the makefile and run when it is executed.
add_custom_command(TARGET build_deploy POST_BUILD
    COMMAND echo "Deploying executable to remote target ${BOARD_IP}:${PORT_NO} ${TARGET_DIR} "
     COMMAND scp ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} root@${BOARD_IP}:${TARGET_DIR}
    # using sshpass for connections that require a password 
    #COMMAND sshpass -p "root" scp ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} root@${BOARD_IP}:${TARGET_DIR}
)

# add_custom_command(TARGET host_build PRE_BUILD
#     COMMAND echo "host build "
#     COMMAND cmake -P "${CMAKE_CURRENT_SOURCE_DIR}/set_host_build.cmake"
# )

# add_custom_command(TARGET build_debug POST_BUILD
#     COMMAND echo "Deploying executable to remote target ${BOARD_IP}:${PORT_NO} ${TARGET_DIR} "
#     COMMAND scp ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} root@${BOARD_IP}:${TARGET_DIR}
#     COMMAND echo "Starting GDB server...... "

#     COMMAND ssh -t root@${BOARD_IP} 'gdbserver :${PORT_NO} nice -n -20 ./../../${TARGET_DIR}/${PROJECT_NAME}'
# )
# # # Note: To start the application with QSPY, we need to pass the host IP address
# # to the application.
# add_custom_command(TARGET qspy_debug POST_BUILD
#     COMMAND echo "Deploying QSPY enabled executable to remote target ${BOARD_IP}:${PORT_NO} ${TARGET_DIR} 192.168.0.68"
#     COMMAND scp ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} root@${BOARD_IP}:${TARGET_DIR}
#     COMMAND echo "Starting GDB server...... "

#     COMMAND echo "When remote GDB server is running! Start local debugger! "
#     COMMAND ssh -t root@${BOARD_IP} 'gdbserver :${PORT_NO} ./../../${TARGET_DIR}/${PROJECT_NAME} 192.168.0.68 '
# )

# add_custom_command(TARGET unit_test
#     COMMAND echo "  [STARTING UNIT TESTS] "
#     COMMAND echo -e
#     COMMAND echo " [RUNNING TEST] client....."
#     COMMAND echo -e
#     COMMAND cd ${CMAKE_CURRENT_LIST_DIR}/tests/test_client && make -j8
#     COMMAND echo -e
#     COMMAND echo " [RUNNING TEST] test_comms_listener....."
#     COMMAND echo -e
#     COMMAND cd ${CMAKE_CURRENT_LIST_DIR}/tests/test_comms_listener && make -j8
    
# )

##* Ideas on running all the tests in a single command.
# 1) create a custom command without the post build option (POST_BUILD)
# 2) add a local cmake file in each test folder and link these to the main
# The main cmake file will call the sub cmake files and build and run each test
#     This can be done with shell commands to call make in each local cmake file.
# Should wait till the test is complete before calling the next test
# Should report if test pass or fail.