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

#include <NFLogger/StdStreamSink.h>

namespace nativeformat {
namespace logger {

StdStreamSink::StdStreamSink(const std::string &identifier,
                             std::shared_ptr<std::ostream> outs,
                             std::shared_ptr<std::ostream> errs)
    : LogSink(identifier), _out_stream(outs), _err_stream(errs) {}

StdStreamSink::~StdStreamSink() {}

void StdStreamSink::write(const std::string &serialized_msg, Severity level) {
  std::ostream &os = (level == Severity::ERROR) ? *(_err_stream.get()) : *(_out_stream.get());
  write(os, serialized_msg);
}

void StdStreamSink::write(std::ostream &os, const std::string &serialized_msg) {
  os << serialized_msg << std::endl;
}

}  // namespace logger
}  // namespace nativeformat
