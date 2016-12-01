project(wb-mqtt-knx)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
set(CMAKE_CXX_FLAGS "-std=c++0x -O2")
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} -lwbmqtt -lmosquitto -lmosquittopp -leibclient)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

