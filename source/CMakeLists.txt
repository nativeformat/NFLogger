set(NFLogger_HEADERS
  "${NFLOGGER_INCLUDE_DIRECTORY}/NFLogger/Logger.h"
  "${NFLOGGER_INCLUDE_DIRECTORY}/NFLogger/LogInfo.h"
  "${NFLOGGER_INCLUDE_DIRECTORY}/NFLogger/LogInfoHandler.h"
  "${NFLOGGER_INCLUDE_DIRECTORY}/NFLogger/LogSink.h"
  "${NFLOGGER_INCLUDE_DIRECTORY}/NFLogger/StdStreamSink.h")

set(NFLogger_SOURCES
  LogInfo.cpp
  LogSink.cpp
  StdStreamSink.cpp)

add_library(NFLogger ${NFLogger_HEADERS} ${NFLogger_SOURCES})
target_include_directories(NFLogger PUBLIC "${NFLOGGER_INCLUDE_DIRECTORY}")
target_compile_definitions(NFLogger
  INTERFACE "NF_LOG_ERROR"
  INTERFACE "$<$<CONFIG:debug>:NF_LOG_WARN;NF_LOG_INFO>")

add_subdirectory(tests)

