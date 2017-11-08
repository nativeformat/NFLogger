cmake_minimum_required(VERSION 3.5)

project(NFLogger)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
if(LLVM_STDLIB)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ \
  -Wno-tautological-undefined-compare -Wno-shorten-64-to-32")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
endif()

if(USE_UB_SANITIZER)
  message("Using Undefined Behavior Sanitizer")
  execute_process(COMMAND python
    tools/generate-ubsan-blacklist.py "${CMAKE_CURRENT_SOURCE_DIR}/tools"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
  set(UB_SAN_BLACKLIST "${CMAKE_CURRENT_SOURCE_DIR}/tools/ubsan.blacklist")
  execute_process(COMMAND cat "${UB_SAN_BLACKLIST}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
  set(UB_SAN_FLAGS " -g -fsanitize=undefined -fno-omit-frame-pointer")
  set(UB_SAN_FLAGS "${UB_SAN_FLAGS} -fno-sanitize-recover=all")
  set(UB_SAN_FLAGS "${UB_SAN_FLAGS} -fsanitize-blacklist=${UB_SAN_BLACKLIST}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${UB_SAN_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${UB_SAN_FLAGS}")
endif()

if(USE_ADDRESS_SANITIZER)
  message("Using Address & Leak Sanitizer")
  set(
    CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -fsanitize=address -g -fno-omit-frame-pointer")
  set(
    CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -g -fno-omit-frame-pointer")
endif()

if(IOS)
  message("Building a project for iOS")
  set(CMAKE_XCODE_ATTRIBUTE_SDKROOT iphoneos)
  set(CMAKE_XCODE_SUPPORTED_PLATFORMS "iphonesimulator iphoneos")
endif()

set(NFLOGGER_INCLUDE_DIRECTORY
  "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE STRING "NFLogger Includes" FORCE)

add_subdirectory(source)
