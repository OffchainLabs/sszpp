cmake_minimum_required( VERSION 3.5.0 FATAL_ERROR )
project( ssz++ VERSION 0.1 LANGUAGES C CXX )

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Wall -Werror -march=native -O3 -ffast-math -fdiagnostics-color")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -Werror -march=native -g -fdiagnostics-color")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lstdc++")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/.cmake_modules/")

include_directories(${CMAKE_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/lib")

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release )
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set( CMAKE_EXPORT_COMPILE_COMMANDS ON ) #youcompleteme

option(CPPCHECK "Analyze the code with cppcheck")
if(CPPCHECK)
	message(STATUS "Building with cppcheck")
	set(CMAKE_CXX_CPPCHECK "cppcheck")
	list(
		APPEND CMAKE_CXX_CPPCHECK
		"--enable=all"
		"--inline-suppr"
		"--inconclusive"
	)
endif()

find_package(intx CONFIG REQUIRED)
if(intx_FOUND)
	message(STATUS "Found intx")
endif()

find_package(yaml-cpp)
if(NOT yaml-cpp_FOUND)
    message(STATUS "Didn't find yaml-cpp, will not be able to run spectests")
else()
    add_compile_definitions( HAVE_YAML )
    message(STATUS "Found yaml-cpp")
endif()
find_package(Snappy)
if(NOT Snappy_FOUND)
    message(STATUS "Didn't find snappy, will not be able to run spectests")
else() 
    message(STATUS "Found Snappy")
endif()
find_package(Hashtree)
if(Hashtree_FOUND)
	message(STATUS "Found Hashtree")
endif()

option(TIDY "Analyze the code with clang-tidy")
if(TIDY)
	message(STATUS "Building with clang-tidy")
	set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
endif()

add_executable( bench_beacon_state 
                testing/bench_beacon_state.cpp
            )
target_link_libraries(bench_beacon_state hashtree)
add_executable( test_concepts
                testing/concepts_test.cpp )
target_link_libraries( test_concepts hashtree)
add_executable( test_serialize
                testing/serialize_test.cpp )
target_link_libraries( test_serialize hashtree)

if(yaml-cpp_FOUND AND Snappy_FOUND)
    add_executable( spectests
                    testing/spec_test.cpp 
                )
    target_link_libraries(spectests hashtree yaml-cpp snappy )
    add_test( spectests spectests )
endif()
                    
set( SPECTEST_URL "https://github.com/ethereum/consensus-spec-tests/releases/download")
set( SPECTEST_VERSION "v1.4.0-beta.0")
set( SPECTEST_MAINNET_HASH "5eeb9b7b0c882fe11ea14444cec44e9e77655f384d80b6743b1f0f82ec9a6599")

file (DOWNLOAD "${SPECTEST_URL}/${SPECTEST_VERSION}/mainnet.tar.gz" ${CMAKE_SOURCE_DIR}/testing/mainnet.tar.gz
    EXPECTED_HASH SHA256=${SPECTEST_MAINNET_HASH})

add_custom_target( spec_test_unpack 
    DEPENDS ${CMAKE_SOURCE_DIR}/testing/mainnet.tar.gz )

add_custom_command(TARGET spec_test_unpack PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/tests/mainnet
    COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_SOURCE_DIR}/testing/mainnet.tar.gz
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS ${CMAKE_SOURCE_DIR}/testing/mainnet.tar.gz
    COMMENT "Unpacking mainnet.tar.gz"
    VERBATIM
)

add_dependencies( test_serialize spec_test_unpack )

enable_testing()
add_test( concepts test_concepts )
add_test( serialize test_serialize )
