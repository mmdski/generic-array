cmake_minimum_required(VERSION 3.18)

project(generic)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

if(MSVC)
  add_compile_options(/guard:cf /analyze /permissive- /W4 /WX)

  macro(link_math target)
  endmacro()

else()
  add_compile_options(-Wall -Wextra -pedantic -Werror -fsanitize=address)
  add_link_options(-fsanitize=address)

  macro(link_math target)
    target_link_libraries(${target} m)
  endmacro()
endif()

# determine if ASAN and LSAN are available
# Windows compiler
if(MSVC)
  # add ASAN flag https://docs.microsoft.com/en-us/cpp/sanitizers/asan
  set(ASAN_COMPILE_OPTION /fsanitize=address)
  set(ASAN_LINK_OPTION)
  set(ASAN_AVAIL True)
  set(LSAN_AVAIL False)

# macOS
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  # assume Homebrew Clang and has ASAN and LSAN
  if(("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang"))
    set(ASAN_COMPILE_OPTION -fsanitize=address)
    set(ASAN_LINK_OPTION -fsanitize=address)
    set(ASAN_AVAIL True)
    set(LSAN_AVAIL True)

  # Apple Clang has ASAN but not LSAN
  elseif(("${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang"))
    set(ASAN_COMPILE_OPTION -fsanitize=address)
    set(ASAN_LINK_OPTION -fsanitize=address)
    set(ASAN_AVAIL True)
    set(LSAN_AVAIL False)

  # assume anything else on macOS has neither
  else()
    set(ASAN_AVAIL False)
    set(LSAN_AVAIL False)
  endif()

# Linux
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  set(ASAN_COMPILE_OPTION -fsanitize=address)
  set(ASAN_LINK_OPTION -fsanitize=address)
  set(ASAN_AVAIL True)
  set(LSAN_AVAIL True)

# unknown, assume no ASAN or LSAN
else()
  set(ASAN_AVAIL False)
  set(LSAN_AVAIL False)
endif()

if(ASAN_AVAIL)
  set(USING_ASAN True)
  message(STATUS "Using ASAN")
  add_compile_options(${ASAN_COMPILE_OPTION})
  add_link_options(${ASAN_LINK_OPTION})

  if(LSAN_AVAIL)
    set(USING_LSAN True)
    message(STATUS "Using LSAN")
  endif()
endif()

add_executable(generic generic.c)

enable_testing()
include(CTest)

if(USING_ASAN)
  add_test(NAME generic COMMAND generic)

  # detect leaks when using LSAN
  if(USING_LSAN)
    set_tests_properties(generic PROPERTIES ENVIRONMENT
      "ASAN_OPTIONS=detect_leaks=1:abort_on_error=0;LSAN_OPTIONS=suppressions=${CMAKE_CURRENT_SOURCE_DIR}/lsan.supp")
  else()
    set_tests_properties(generic PROPERTIES ENVIRONMENT
      "ASAN_OPTIONS=abort_on_error=0")
  endif()

else()
  add_test(NAME generic COMMAND generic)
endif()
