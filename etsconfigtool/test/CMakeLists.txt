
aux_source_directory(./ TEST_LIST)
aux_source_directory(./mock TEST_LIST)

add_executable(${PROJECT_NAME}_test ${TEST_LIST}
        ../../src/knxgroupobject/datapointid.cpp
        ../../src/knxgroupobject/basedptconfig.cpp
        ../../src/knxgroupobject/dptjsonconfig.cpp
        ../../src/knxgroupobject/dptwbmqttconfig.cpp
        ../src/etsconfigtool.cpp
        ../src/mqttknxconfig.cpp
        ../../src/knxgroupaddress.cpp
        ../../src/knxexception.cpp
        ../../src/knxutils.cpp
        )

target_include_directories(${PROJECT_NAME}_test PRIVATE ../include)

target_link_libraries(${PROJECT_NAME}_test gtest gtest_main gmock pthread tinyxml2 wbmqtt1)

add_test(${PROJECT_NAME}_test ${PROJECT_NAME}_test)
