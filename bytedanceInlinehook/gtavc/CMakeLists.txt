cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_VERBOSE_MAKEFILE on)
set(TARGET "pgod")
set(3rd_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../3rd)

add_library(lib_shadowhook STATIC IMPORTED)
set_target_properties(lib_shadowhook PROPERTIES IMPORTED_LOCATION ${3rd_DIR}/libs/${ANDROID_ABI}/libshadowhook.a)

set(CMAKE_BUILD_TYPE Release)  
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")  

message("########## build pgod")
add_library(${TARGET} SHARED 
			main.cpp
			common.cpp
			cheatSDK.cpp)
			
#add_executable(${TARGET}
#			main.cpp
#			common.cpp)
			
target_include_directories(${TARGET} PRIVATE ${3rd_DIR}/include)

target_compile_features(${TARGET} PUBLIC c_std_17)

target_link_libraries(${TARGET} lib_shadowhook log)
