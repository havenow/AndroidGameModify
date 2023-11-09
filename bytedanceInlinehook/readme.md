```
将shadowhook当成三方库来使用，修改如下：
$ git diff shadowhook/src/main/cpp/CMakeLists.txt
diff --git a/shadowhook/src/main/cpp/CMakeLists.txt b/shadowhook/src/main/cpp/CMakeLists.txt
index 8f3b586..56f849b 100644
--- a/shadowhook/src/main/cpp/CMakeLists.txt
+++ b/shadowhook/src/main/cpp/CMakeLists.txt
@@ -9,12 +9,19 @@ endif()

 set(TARGET "shadowhook")
 file(GLOB SRC *.c arch/${ARCH}/*.c common/*.c third_party/*/*.c)
-add_library(${TARGET} SHARED ${SRC})
+add_library(${TARGET} STATIC ${SRC})
 target_compile_features(${TARGET} PUBLIC c_std_17)
 target_compile_options(${TARGET} PUBLIC -std=c17 -Weverything -Werror)
 target_include_directories(${TARGET} PUBLIC . include arch/${ARCH} common third_party/xdl third_party/bsd third_party/lss)
 target_link_libraries(${TARGET} log)

+set(3rd_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../3rd)
+set_target_properties(${TARGET}
+                      PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${3rd_DIR}/libs/${ANDROID_ABI}") # (static)ARCHIVE_OUTPUT_DIRECTORY (shared)LIBRARY_OUTPUT_DIRECTORY (exe)RUNTIME_OUTPUT_DIRECTORY
+add_custom_command(TARGET ${TARGET} POST_BUILD
+                   COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/include/shadowhook.h" "${3rd_DIR}/include/shadowhook.h"
+                   COMMENT "Copying ${TARGET} to output directory")
+
 if(USEASAN)
     target_compile_options(${TARGET} PUBLIC -fsanitize=address -fno-omit-frame-pointer)
     target_link_options(${TARGET} PUBLIC -fsanitize=address)

```
