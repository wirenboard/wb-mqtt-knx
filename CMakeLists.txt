cmake_minimum_required(VERSION 3.7)

project(wb-mqtt-knx VERSION 1.0.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_options(-O0 -Wall -Werror -pedantic -gdwarf-3 --coverage)
else ()
    add_compile_options(-O2 -Wall -Werror -pedantic -DNDEBUG)
endif ()

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

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
