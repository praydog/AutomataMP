cmake_minimum_required(VERSION 2.8)

project("enetpp sample app")

add_subdirectory("${ENET_ROOT_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/enet")

include_directories("${PROJECT_SOURCE_DIR}/../include")
include_directories("${ENET_ROOT_PATH}/include")

add_executable(enetpp main.cpp)

target_link_libraries(enetpp enet)

if(WIN32)
	target_link_libraries(enetpp winmm wsock32 Ws2_32)
endif(WIN32)
