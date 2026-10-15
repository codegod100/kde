include(ECMAddTests)
find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Test)

ecm_add_test(whitelisttest.cpp
    TEST_NAME whitelisttest
    LINK_LIBRARIES systemtray_static Qt::Test
)
target_compile_definitions(whitelisttest PRIVATE SCHEMA_FILE="${CMAKE_SOURCE_DIR}/src/main.xml")
