cmake_minimum_required ( VERSION 2.8.7 )

set ( CMAKE_CXX_STANDARD 17 )
set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall" )
set ( CMAKE_CXX_COMPILER g++-mp-8 )

project ( algolib )
enable_testing ( )

file ( GLOB BOOST_LIBS_INCLUDES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "boost/libs/*/include" "boost/libs/*/*/include" )
set ( BOOST_INCLUDE_DIRS ${BOOST_LIBS_INCLUDES} )

include_directories ( "include" )

file ( GLOB TESTS_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} tests/*.cpp )
foreach ( SOURCE_FILE_NAME ${TESTS_SRCS} )
    #Extract the filename without an extension (NAME_WE)
    get_filename_component ( TEST_NAME_SUFFIX ${SOURCE_FILE_NAME} NAME_WE )
    set ( TEST_EXECUTABLE_NAME "test_${TEST_NAME_SUFFIX}")
    add_executable ( ${TEST_EXECUTABLE_NAME} ${SOURCE_FILE_NAME} )
    target_include_directories ( ${TEST_EXECUTABLE_NAME} SYSTEM PRIVATE ${BOOST_INCLUDE_DIRS} )
    set_target_properties ( ${TEST_EXECUTABLE_NAME}
        PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests )
    file(READ "${SOURCE_FILE_NAME}" SOURCE_FILE_CONTENTS)
    foreach ( PATTERN "BOOST_AUTO_TEST_CASE" "BOOST_DATA_TEST_CASE" "BOOST_AUTO_TEST_CASE_TEMPLATE")
        string ( REGEX MATCHALL "${PATTERN}\\( *([A-Za-z_0-9]+) *[\\),]"
               FOUND_TESTS ${SOURCE_FILE_CONTENTS} )

        foreach ( HIT ${FOUND_TESTS})
            string ( REGEX REPLACE ".*\\( *([A-Za-z_0-9]+) *[\\),].*" "\\1" TEST_NAME ${HIT} )

            add_test ( NAME "${TEST_EXECUTABLE_NAME}.${TEST_NAME}"
                       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests
                       COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tests/${TEST_EXECUTABLE_NAME}
                       --run_test=${TEST_NAME}* )
        endforeach()
    endforeach ( PATTERN )
endforeach ( SOURCE_FILE_NAME )
