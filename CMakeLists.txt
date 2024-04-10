cmake_minimum_required(VERSION 3.26)

project(
  nircc
  VERSION 0.1
  LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# TODO: Required to locate 'LLVM-Config.cmake'
set(LLVM_DIR "/opt/local/libexec/llvm-15/lib/cmake/llvm")
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_library(
  nirc_lib STATIC
  src/lib/Assert.cc
  src/lib/Attribute.cc
  src/lib/AttributeSet.cc
  src/lib/CodeGenerator.cc
  src/lib/Decoder.cc
  src/lib/Deserializer.cc
  src/lib/File.cc
  src/lib/Signature.cc
  src/lib/Symbol.cc
  src/lib/Type.cc
  src/lib/Value.cc
)
include_directories(nirc_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
llvm_map_components_to_libnames(llvm_libs bitwriter core codegen support)
target_link_libraries(nirc_lib ${llvm_libs})

add_executable(nirc src/main.cc)
target_compile_options(nirc PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

target_link_libraries(nirc PRIVATE nirc_lib)

