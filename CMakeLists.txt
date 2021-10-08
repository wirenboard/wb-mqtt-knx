execute_process(COMMAND /usr/bin/which gcc-4.7 RESULT_VARIABLE IS_GCC47)

if (${IS_GCC47} STREQUAL "0")
    set(CMAKE_C_COMPILER gcc-4.7)
    set(CMAKE_CXX_COMPILER g++-4.7)
else ()
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
endif ()

message("CMAKE_BUILD_TYPE = " ${CMAKE_BUILD_TYPE})

project(wb-mqtt-knx)

cmake_minimum_required(VERSION 2.8)
aux_source_directory(src/ SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} -lwbmqtt1 -leibclient -lpthread)

# Adding unit tests
enable_testing()
aux_source_directory(test/ TEST_LIST)
add_executable(${PROJECT_NAME}_test ${TEST_LIST}
        src/knxtelegram.cpp
        src/knxexception.cpp
        src/knxconverter.cpp
        )
target_link_libraries(${PROJECT_NAME}_test -lgtest -lgtest_main -lwbmqtt1 -lpthread)
add_test(telegram_test ${PROJECT_NAME}_test)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
        DEPENDS ${PROJECT_NAME}_test)


set(CMAKE_CXX_FLAGS_RELEASE "-std=c++1y -O2 -Wall -Werror -pedantic -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-std=c++1y -O0 -Wall -Werror -pedantic -gdwarf-3 --coverage")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_RELEASE})


install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
