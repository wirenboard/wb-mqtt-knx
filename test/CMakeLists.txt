
enable_testing()
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
        ../src/knxgroupobject/dpt10.cpp
        ../src/knxgroupobject/dpt11.cpp
        ../src/knxgroupobject/dpt12.cpp
        ../src/knxgroupobject/dpt13.cpp
        ../src/knxgroupobject/dpt14.cpp
        ../src/knxgroupobject/dpt17.cpp
        ../src/knxgroupobject/dpt18.cpp
        ../src/knxgroupobject/dpt20.cpp
        ../src/knxgroupobject/dptraw.cpp
        ../src/knxutils.cpp
        ../src/knxgroupobjectcontroller.cpp
        )

target_link_libraries(${PROJECT_NAME}_test gtest gtest_main gmock wbmqtt1 pthread)


if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(${PROJECT_NAME}_test gcov)
endif ()

add_test(${PROJECT_NAME}_test ${PROJECT_NAME}_test)