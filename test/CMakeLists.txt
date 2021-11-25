
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
        ../src/knxgroupobject/dpt1.cpp
        ../src/knxgroupobject/dpt5.cpp
        ../src/knxgroupobject/dpt6.cpp
        ../src/knxgroupobject/dpt9.cpp
        ../src/knxgroupobject/dptraw.cpp
        ../src/knxutils.cpp
        )

target_link_libraries(${PROJECT_NAME}_test gtest gtest_main gmock wbmqtt1 pthread)


if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(${PROJECT_NAME}_test gcov)
endif ()

add_test(${PROJECT_NAME}_test ${PROJECT_NAME}_test)