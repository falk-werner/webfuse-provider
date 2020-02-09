cmake_minimum_required (VERSION 3.10)
project(webfuse-provider VERSION 0.1.0 DESCRIPTION "Webfuse provider")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LWS REQUIRED libwebsockets)
pkg_check_modules(WEBFUSE REQUIRED libwebfuse-provider)

include_directories(
	"src"
	${WEBFUSE_INCLUDE_DIRS}
)

link_directories(
    ${WEBFUSE_LIBRARY_DIRS}
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(C_WARNINGS -Wall -Wextra)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(webfuse-provider-app
	src/main.c
)

set_target_properties(webfuse-provider-app PROPERTIES OUTPUT_NAME webfuse-provider)

target_link_libraries(webfuse-provider-app PUBLIC webfuse-provider ${WEBFUSE_LIBRARIES})

# static-filesystem-provider

add_executable(static-filesystem-provider
	src/static_filesystem.c
)

target_link_libraries(static-filesystem-provider PUBLIC webfuse-provider ${WEBFUSE_LIBRARIES})

