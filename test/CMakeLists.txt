
aux_source_directory(./ TEST_LIST)
aux_source_directory(./mock TEST_LIST)

add_executable(${PROJECT_NAME}_test ${TEST_LIST}
        ../src/knxtelegram.cpp
        ../src/knxexception.cpp
        ../src/knxconverter.cpp
        ../src/knxtelegramtpdu.cpp
        ../src/configurator.cpp
        ../src/knxgroupaddress.cpp
        ../src/knxgroupobject/dptbaseu8.cpp
        ../src/knxgroupobject/dpt1.cpp
        ../src/knxgroupobject/dpt2.cpp
        ../src/knxgroupobject/dpt3.cpp
        ../src/knxgroupobject/dpt4.cpp
        ../src/knxgroupobject/dpt5.cpp
        ../src/knxgroupobject/dpt6.cpp
        ../src/knxgroupobject/dpt7.cpp
        ../src/knxgroupobject/dpt8.cpp
        ../src/knxgroupobject/dpt9.cpp
        ../src/knxgroupobject/dpt12.cpp
        ../src/knxgroupobject/dpt13.cpp
        ../src/knxgroupobject/dpt14.cpp
        ../src/knxgroupobject/dpt20.cpp
        ../src/knxgroupobject/dptraw.cpp
        ../src/knxgroupobject/dptjson.cpp
        ../src/knxgroupobject/dptjsonfield.cpp
        ../src/knxgroupobject/dptjsonbuilder.cpp
        ../src/knxgroupobject/dptwbmqttbuilder.cpp
        ../src/knxgroupobject/datapointid.cpp
        ../src/knxgroupobject/datapointutils.cpp
        ../src/knxutils.cpp
        ../src/knxgroupobjectcontroller.cpp
        ../src/knxgroupobject/basedptconfig.cpp
        ../src/knxgroupobject/dptjsonconfig.cpp
        ../src/knxgroupobject/dptwbmqttconfig.cpp
        ../src/knxindinvidualaddress.cpp
        )

target_link_libraries(${PROJECT_NAME}_test gtest gtest_main gmock wbmqtt1 pthread)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(${PROJECT_NAME}_test gcov)
endif ()

add_test(${PROJECT_NAME}_test ${PROJECT_NAME}_test)
