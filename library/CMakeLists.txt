# Copyright 2020 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.10.2)

# TCSAFLUSH fails on Android 8.0 to 11 due to SELinux denial, so use TCSADRAIN instead.
# See also https://github.com/termux/termux-packages/issues/1359 .
set(LINENOISE_C_INPUT src/main/jni/external/linenoise/linenoise.c)
set(LINENOISE_C_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/linenoise_patched.c")
set(LINENOISE_C_PATCH src/main/jni/linenoise.c.patch)
find_package(Patch REQUIRED)
add_custom_command(OUTPUT "${LINENOISE_C_OUTPUT}"
        COMMAND "${CMAKE_COMMAND}" -E copy "${LINENOISE_C_INPUT}" "${LINENOISE_C_OUTPUT}"
        COMMAND "${Patch_EXECUTABLE}" "${LINENOISE_C_OUTPUT}" "${LINENOISE_C_PATCH}"
        MAIN_DEPENDENCY "${LINENOISE_C_INPUT}"
        DEPENDS "${LINENOISE_C_PATCH}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating linenoise_patched.c"
        VERBATIM)

add_library(linenoise STATIC "${LINENOISE_C_OUTPUT}")
target_include_directories(linenoise PUBLIC src/main/jni/external/linenoise)

find_library(LOG_LIBRARY log)
add_library(linenoise-jni SHARED src/main/jni/linenoise-jni.c)
target_link_libraries(linenoise-jni linenoise ${LOG_LIBRARY})
