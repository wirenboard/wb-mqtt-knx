cmake_minimum_required(VERSION 3.7)

project(wb-mqtt-knx)

set(DEFAULT_CONFIG_FILE_PATH "/etc/wb-mqtt-knx.conf" CACHE STRING "Set default json config path")
set(DEFAULT_CONFIG_SCHEMA_FILE_PATH "/usr/share/wb-mqtt-confed/schemas/wb-mqtt-knx.schema.json" CACHE STRING "Set default json config schema path")
set(DEFAULT_CONFIG_JSON_DATAPOINT_FILE_PATH "/usr/share/wb-mqtt-knx/wb-mqtt-knx-jsondpt.conf" CACHE STRING "Set default json datapoint config path")

execute_process (
        COMMAND bash -c "head -1 ${PROJECT_SOURCE_DIR}/debian/changelog | awk '{ printf $2; }' | sed 's/[\(\)]//g'"
        OUTPUT_VARIABLE DEB_VERSION
)

# add the binary tree to the search path for include files
# so that we will find config.h
include_directories("${PROJECT_BINARY_DIR}")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -Werror -pedantic)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_options(-O0 -gdwarf-3 --coverage)
    add_link_options(--coverage)
else ()
    add_compile_options(-O2 -DNDEBUG)
endif ()

aux_source_directory(src/ SRC_LIST)
aux_source_directory(src/knxgroupobject SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} wbmqtt1 eibclient pthread)

# Adding unit tests
include (CTest)
add_subdirectory(test)

# Added ets.xml <-> wb-mqtt-knx.conf tool
add_subdirectory(etsconfigtool)

# tell CMake to generate config.h
configure_file (
        "${PROJECT_SOURCE_DIR}/src/config.h.in"
        "${PROJECT_BINARY_DIR}/config.h"
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES ${PROJECT_SOURCE_DIR}/wb-mqtt-knx.schema.json DESTINATION /usr/share/wb-mqtt-confed/schemas)
install(FILES ${PROJECT_SOURCE_DIR}/wb-mqtt-knx.conf DESTINATION /etc)
install(FILES ${PROJECT_SOURCE_DIR}/wb-mqtt-knx-jsondpt.conf DESTINATION /usr/share/wb-mqtt-knx)
install(FILES ${PROJECT_SOURCE_DIR}/wb-mqtt-knx.wbconfigs DESTINATION /etc/wb-configs.d RENAME 42wb-mqtt-knx)
install(DIRECTORY DESTINATION /var/lib/wb-mqtt-knx)
