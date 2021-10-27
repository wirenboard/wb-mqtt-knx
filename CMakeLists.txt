cmake_minimum_required(VERSION 3.7)

project(wb-mqtt-knx VERSION 1.0.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -Werror -pedantic)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_options(-O0 -gdwarf-3 --coverage)
else()
    add_compile_options(-O2 -DNDEBUG)
endif()

aux_source_directory(src/ SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} wbmqtt1 eibclient pthread)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(${PROJECT_NAME} gcov)
endif()

# Adding unit tests
enable_testing()
aux_source_directory(test/ TEST_LIST)
add_executable(${PROJECT_NAME}_test ${TEST_LIST}
        src/knxtelegram.cpp
        src/knxexception.cpp
        src/knxconverter.cpp
        )

target_link_libraries(${PROJECT_NAME}_test gtest gtest_main wbmqtt1 pthread)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(${PROJECT_NAME}_test gcov)
endif()

add_test(${PROJECT_NAME}_test ${PROJECT_NAME}_test)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)