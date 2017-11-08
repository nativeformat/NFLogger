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
#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>

namespace nativeformat {
namespace logger {

const char DEFAULT_DOMAIN[] = "";
const int DEFAULT_CODE = 0;
const char DEFAULT_MSG[] = "No details provided";

// Right now only REPORTING, ERROR, WARNING, and INFO are used
enum class Severity {
  REPORTING = 0,
  EMERGENCY,
  ALERT,
  CRITICAL,
  ERROR,
  WARNING,
  NOTICE,
  INFO,
  DEBUG
};

std::string severityString(Severity level);

// All the parts of the LogInfo class that SHOULD NOT
// need to be specialized go in here
template <typename PayloadType>
class LogInfoBase {
 public:
  LogInfoBase(std::unique_ptr<PayloadType> payload,
              Severity level = Severity::INFO,
              std::string domain = DEFAULT_DOMAIN)
      : _domain(std::move(domain)), _payload(std::move(payload)), _level(level) {}
  virtual ~LogInfoBase() {}

  void setLevel(Severity new_level) { _level = new_level; }
  Severity getLevel() const { return _level; }

 protected:
  std::string _domain;
  const std::unique_ptr<PayloadType> _payload;
  Severity _level;
};

// Class to be specialized
template <typename PayloadType, typename BaseType = PayloadType>
class LogInfo : public LogInfoBase<PayloadType> {
 public:
  LogInfo(std::unique_ptr<PayloadType> payload,
          Severity level = Severity::INFO,
          std::string domain = DEFAULT_DOMAIN)
      : LogInfoBase<PayloadType>(std::move(payload), level, domain) {}
  virtual ~LogInfo(){};

  // Specializations must implement these serialization methods
  std::string toString() const { return ""; };
  std::string toProto() const { return ""; };
  std::string toJson() const { return ""; };
};

// Templates for easier LogInfo creation
template <typename PayloadType, typename... Args>
std::unique_ptr<LogInfo<PayloadType>> makeLogInfo(std::string domain,
                                                  Severity level,
                                                  Args &&... args) {
  std::unique_ptr<PayloadType> payload =
      std::unique_ptr<PayloadType>(new PayloadType(std::forward<Args>(args)...));
  std::unique_ptr<LogInfo<PayloadType>> li = std::unique_ptr<LogInfo<PayloadType>>(
      new LogInfo<PayloadType>(std::move(payload), level, domain));
  return li;
}

template <typename PayloadType>
std::unique_ptr<LogInfo<PayloadType>> fillLogInfo(std::string domain,
                                                  std::unique_ptr<PayloadType> p,
                                                  Severity level = Severity::INFO) {
  std::unique_ptr<LogInfo<PayloadType>> li =
      std::unique_ptr<LogInfo<PayloadType>>(new LogInfo<PayloadType>(std::move(p), level, domain));
  return li;
}

// String specialization (no reason to wrap strings in unique_ptrs)
template <>
class LogInfo<std::string> : public LogInfoBase<std::string> {
 public:
  // Allow copying for boost future reasons
  LogInfo(const LogInfo &li)
      : LogInfoBase<std::string>(nullptr, li._level, li._domain), _str_payload(li._str_payload) {}
  LogInfo(std::string payload, Severity level, std::string domain = DEFAULT_DOMAIN)
      : LogInfoBase<std::string>(nullptr, level, domain), _str_payload(std::move(payload)) {}
  virtual ~LogInfo(){};

  std::string toProto() const { return ""; }
  std::string toJson() const { return ""; }
  std::string toString() const {
    std::string prefix = severityString(_level);
    std::stringstream ss;
    ss << prefix << ": ";
    if (!this->_domain.empty()) {
      ss << this->_domain << ": ";
    }
    ss << _str_payload;
    return ss.str();
  }

 private:
  std::string _str_payload;
};

}  // namespace logger
}  // namespace nativeformat
