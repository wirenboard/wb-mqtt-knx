project(wb-knx-ets-tool)

find_package(PkgConfig)

pkg_search_module(TinyXML2 REQUIRED tinyxml2)

aux_source_directory(src/ SRC_FILES_LIST)
aux_source_directory(include/ SRC_FILES_LIST)

add_executable(${PROJECT_NAME} ${SRC_FILES_LIST}
        ../../src/knxgroupobject/datapointid.cpp
        ../../src/knxgroupobject/basedptconfig.cpp
        ../../src/knxgroupobject/dptjsonconfig.cpp
        ../../src/knxgroupobject/dptwbmqttconfig.cpp
        ../src/knxgroupaddress.cpp
        ../src/knxexception.cpp
        ../../src/knxutils.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} gcov tinyxml2 wbmqtt1)

# Adding unit tests
add_subdirectory(test)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
