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

#include <map>
#include <memory>
#include <typeinfo>
#include <NFLogger/LogInfo.h>
#include <NFLogger/LogSink.h>

namespace nativeformat {
namespace logger {

enum class LogInfoFormat { STRING, PROTOBUF, JSON };

template <typename PayloadType, typename BaseType = PayloadType>
class LogInfoHandler {
 public:
  LogInfoHandler() {}
  virtual ~LogInfoHandler() {}

  virtual void handle(const LogInfo<PayloadType, BaseType> *li) {
    for (auto sink_pair : _sinks) {
      std::string msg;
      switch (sink_pair.second) {
        case LogInfoFormat::STRING:
          msg = li->toString();
          break;
        case LogInfoFormat::PROTOBUF:
          msg = li->toProto();
          break;
        case LogInfoFormat::JSON:
          msg = li->toJson();
      }
      sink_pair.first->write(msg, li->getLevel());
    }
  }

  virtual void addSink(std::shared_ptr<LogSink> sink, LogInfoFormat format) {
    _sinks[sink] = format;
  }

 protected:
  std::map<const std::shared_ptr<LogSink>, LogInfoFormat> _sinks;
};

}  // namespace logger
}  // namespace nativeformat
