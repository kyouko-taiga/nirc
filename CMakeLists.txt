cmake_minimum_required(VERSION 3.26)

project(
  nircc
  VERSION 0.1
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(
  nirc_lib STATIC
  src/lib/Assert.cc
  src/lib/Attribute.cc
  src/lib/AttributeSet.cc
  src/lib/Decoder.cc
  src/lib/Deserializer.cc
  src/lib/File.cc
  src/lib/Signature.cc
  src/lib/Symbol.cc
  src/lib/Type.cc
  src/lib/Value.cc
)
include_directories(nirc_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(nirc src/main.cc)
target_compile_options(nirc PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
target_link_libraries(nirc PRIVATE nirc_lib)
