/*
 * Copyright (c) 2017 Spotify AB.
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

#ifdef NF_LOG_ERROR
#define ERROR(l, i) l._error_wrapper(i)
#else
#define ERROR(l, i)
#endif

#ifdef NF_LOG_WARN
#define WARN(l, i) l._warn_wrapper(i)
#else
#define WARN(l, i)
#endif

#ifdef NF_LOG_INFO
#define INFO(l, i) l._info_wrapper(i)
#else
#define INFO(l, i)
#endif

// Reporting should always be enabled, but not for strings
#define REPORT(l, i) l._report_wrapper(i)

#include <NFLogger/LogInfo.h>
#include <NFLogger/LogInfoHandler.h>

#include <cstdio>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <thread>
#include <tuple>
#include <type_traits>
#include <vector>

namespace nativeformat {
namespace logger {

// Some recursive variadic template metaprogramming to
// reproduce std::tuple std::get functionality from C++14 :D
template <int Index, class Search, class First, class... Types>
struct get_internal {
  typedef typename get_internal<Index + 1, Search, Types...>::type type;
  static constexpr int index = Index;
};

template <int Index, class Search, class... Types>
struct get_internal<Index, Search, Search, Types...> {
  typedef get_internal type;
  static constexpr int index = Index;
};

template <class T, class... Types>
T &get(std::tuple<Types...> &tuple) {
  return std::get<get_internal<0, T, Types...>::type::index>(tuple);
}

template <typename... HandlerTypes>
class Logger {
 public:
  Logger() {}
  virtual ~Logger() {}

  // Wrappers defined for a variety of types to make the macros friendly
  void _error_wrapper(std::string s) {
    auto li = std::unique_ptr<LogInfo<std::string>>(new LogInfo<std::string>(s, Severity::ERROR));
    _error_wrapper(li);
  }
  void _warn_wrapper(std::string s) {
    auto li = std::unique_ptr<LogInfo<std::string>>(new LogInfo<std::string>(s, Severity::WARNING));
    _warn_wrapper(li);
  }
  void _info_wrapper(std::string s) {
    auto li = std::unique_ptr<LogInfo<std::string>>(new LogInfo<std::string>(s, Severity::INFO));
    _info_wrapper(li);
  }

  template <typename T, typename B = T>
  void _error_wrapper(const LogInfo<T, B> *li) {
    LogInfoHandler<T, B> &handler = get<LogInfoHandler<T, B>>(_handlers);
    handler.handle(li);
  }
  template <typename T, typename B = T>
  void _warn_wrapper(const LogInfo<T, B> *li) {
    LogInfoHandler<T, B> &handler = get<LogInfoHandler<T, B>>(_handlers);
    handler.handle(li);
  }
  template <typename T, typename B = T>
  void _info_wrapper(const LogInfo<T, B> *li) {
    LogInfoHandler<T, B> &handler = get<LogInfoHandler<T, B>>(_handlers);
    handler.handle(li);
  }
  template <typename T, typename B = T>
  void _report_wrapper(const LogInfo<T, B> *li) {
    LogInfoHandler<T, B> &handler = get<LogInfoHandler<T, B>>(_handlers);
    handler.handle(li);
  }

  template <typename T, typename B = T>
  void _error_wrapper(std::unique_ptr<LogInfo<T, B>> &li) {
    _error_wrapper(li.get());
  }
  template <typename T, typename B = T>
  void _warn_wrapper(std::unique_ptr<LogInfo<T, B>> &li) {
    _warn_wrapper(li.get());
  }
  template <typename T, typename B = T>
  void _info_wrapper(std::unique_ptr<LogInfo<T, B>> &li) {
    _info_wrapper(li.get());
  }
  template <typename T, typename B = T>
  void _report_wrapper(std::unique_ptr<LogInfo<T, B>> &li) {
    _report_wrapper(li.get());
  }

  template <typename T, typename B = T>
  void addSink(std::shared_ptr<LogSink> sink, LogInfoFormat fmt) {
    std::lock_guard<std::mutex> lock(_instance_mutex);
    get<LogInfoHandler<T, B>>(_handlers).addSink(sink, fmt);
  }

 private:
  // static mutex to prevent collisions in case sinks are shared
  static std::mutex &get_log_mutex() {
    static std::mutex log_mutex;
    return log_mutex;
  }

  // private mutex for this instance only
  std::mutex _instance_mutex;

 protected:
  std::tuple<HandlerTypes...> _handlers;
};

}  // namespace logger
}  // namespace nativeformat
