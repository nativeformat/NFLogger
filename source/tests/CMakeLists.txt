add_executable(NFLoggerTest ${NFLogger_HEADERS} NFLoggerTest.cpp)
target_include_directories(NFLoggerTest PUBLIC "${NFLOGGER_INCLUDE_DIRECTORY}")

# By default, always log errors
# Only log info and errors for debug builds
target_compile_definitions(NFLoggerTest
  INTERFACE "NF_LOG_ERROR"
  INTERFACE "$<$<CONFIG:debug>:NF_LOG_WARN;NF_LOG_INFO>")

target_link_libraries(NFLoggerTest NFLogger pthread)
