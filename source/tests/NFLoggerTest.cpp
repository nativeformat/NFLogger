/*
 * Copyright (c) 2018 Spotify AB.
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <NFLogger/LogSink.h>
#include <NFLogger/Logger.h>
#include <NFLogger/StdStreamSink.h>

#include <iostream>
#include <sstream>

namespace nativeformat {
namespace logger {

// Temporary class for testing until we have real messages
class TestMessage {
 public:
  TestMessage(int code, std::string context) : _code(code), _context(std::move(context)) {}
  virtual ~TestMessage() {}
  friend std::ostream &operator<<(std::ostream &os, const TestMessage &tm);

  const int _code;
  const std::string _context;
};

std::ostream &operator<<(std::ostream &os, const TestMessage &tm) {
  os << tm._code << ": " << tm._context;
  return os;
}

// TestMessage specialization (a temporary stand-in for protobuf)
template <>
class LogInfo<TestMessage> : public LogInfoBase<TestMessage> {
 public:
  LogInfo(std::unique_ptr<TestMessage> payload, Severity level, std::string domain = DEFAULT_DOMAIN)
      : LogInfoBase<TestMessage>(std::move(payload), level, domain) {}
  virtual ~LogInfo(){};

  std::string toString() const { return ""; }
  std::string toProto() const { return ""; }
  std::string toJson() const {
    std::stringstream ss;
    ss << "{";
    ss << "\"domain\":\"" << _domain << "\",";
    ss << "\"code\":" << _payload->_code << ",";
    ss << "\"context\":\"" << _payload->_context << "\"";
    ss << "}";
    return ss.str();
  }
};

}  // namespace logger
}  // namespace nativeformat

int main(int argc, char *argv[]) {
  using namespace nativeformat::logger;
  std::stringstream outBuf, errBuf, ss;
  const std::string testDomain = "com.nativeformat.logger.test";
  ss << "Error: String error\n"
     << "{\"domain\":\"com.nativeformat.logger.test\",\"code\":3,\"context\":"
        "\"Doing stuff\"}\n";

#ifdef NF_LOG_INFO
  const std::string expectedInfo = "Info: Info from default domain\n";
#else
  const std::string expectedInfo = "";
#endif
  const std::string expectedError = ss.str();

  // Create Logger and Sinks
  Logger<LogInfoHandler<std::string>, LogInfoHandler<TestMessage>> testLogger;
  auto stream_sink =
      std::make_shared<StdStreamSink>("Test stdout",
                                      std::shared_ptr<std::ostream>(&outBuf, [](void *) {}),
                                      std::shared_ptr<std::ostream>(&errBuf, [](void *) {}));
  testLogger.addSink<std::string>(stream_sink, LogInfoFormat::STRING);
  testLogger.addSink<TestMessage>(stream_sink, LogInfoFormat::JSON);

  INFO(testLogger, "Info from default domain");
  ERROR(testLogger, "String error");
  auto li = makeLogInfo<TestMessage>(testDomain, Severity::ERROR, 3, "Doing stuff");
  ERROR(testLogger, li);

  int ret = 0;
  std::stringstream msg;
  if (outBuf.str() != expectedInfo) {
    ret = -1;
    msg << outBuf.str() << " is not equal to\n" << expectedInfo << std::endl;
  }
  if (errBuf.str() != expectedError) {
    ret = -1;
    msg << errBuf.str() << " is not equal to\n" << expectedError << std::endl;
  }

  if (ret) {
    std::cerr << msg.str();
  } else {
    std::cout << "NFLoggingTest OK" << std::endl;
  }

  return ret;
}
