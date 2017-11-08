<img alt="NFLogger" src="NFLogger.png" width="100%" max-width="888">

[![CircleCI](https://circleci.com/gh/spotify/NFLogger/tree/master.svg?style=svg)](https://circleci.com/gh/spotify/NFLogger/tree/master)
[![License](https://img.shields.io/github/license/spotify/NFLogger.svg)](LICENSE)
[![Spotify FOSS Slack](https://slackin.spotify.com/badge.svg)](https://slackin.spotify.com)
[![Readme Score](http://readme-score-api.herokuapp.com/score.svg?url=https://github.com/spotify/NFLogger)](http://clayallsopp.github.io/readme-score?url=https://github.com/spotify/NFLogger)

A C++ threadsafe logging framework with customisable messages and sinks.

- [x] 📱 iOS 9.0+
- [x] 💻 OS X 10.11+
- [x] 🐧 Ubuntu Bionic 18.04+ (clang or gcc)

## Raison D'être :thought_balloon:
When developing C++ projects, we often found we had a need for a customisable logging library that would fit all our uses. For example in some of our builds, we just want logs to be printed to standard out, sometimes we want certain log messages to post to backends with different quality of services (e.g. can the log be dropped if the backend is unreachable?), and sometimes we want a log to write directly to a disk in an encrypted format that only we can utilise. Sometimes these logs needed to be text, sometimes they needed to be JSON, and sometimes they needed to be protobuf. So we made this framework to be as generic as possible, allowing the user to define their own log types, log from anywhere in the app (whether its the audio thread or the main thread), define the severity of the log and have different sinks perform different actions according to the log being processed.

## Architecture :triangular_ruler:
`NFLogger` is designed as a C++ interface that allows the user to implement log sinks and log messages themselves by conforming to virtual C++ classes. The logger is then accessed by very simple C macros such as `ERROR(...)` or `INFO(...)` to post messages from anywhere in your app. The reason for the macro choice was to allow the exclusion of these log messages in certain builds of our applications; for example you may not want to log encryption information in the production app at all to the point where you don't even want such logging compiled in, but you may want to log this in a debug version of the app.

## Installation :inbox_tray:
`NFLogger` is a cmake project, while you can feel free to download the prebuilt static libraries it is recommended to use cmake to install this project into your wider project. In order to add this into a wider Cmake project, simply add the following line to your `CMakeLists.txt` file:
```
add_subdirectory(NFLogger)
```

It should be noted that some log messages are dependent on compiler definitions to work:

| Message       | Compiler Definition |
| ------------- |:-------------------:|
| INFO          | NF_LOG_INFO         |
| WARN          | NF_LOG_WARN         |
| ERROR         | NF_LOG_ERROR        |

### For iOS/OSX
Generate an Xcode project from the Cmake project like so:

```shell
$ mkdir build
$ cd build
$ cmake .. -GXcode
```

### For Linux
Generate a Ninja project from the Cmake project like so:

```shell
$ mkdir build
$ cd build
$ cmake .. -GNinja
```

## Usage example :eyes:
For examples of this in use, see the demo program `src/tests/NFLoggerTest.cpp`. The API itself is rather small, it generally requires the user to implement virtual classes for more advanced logging (such as encrypted on disk and posting to HTTPS endpoints).

```C++
#include <NFLogger/LogSink.h>
#include <NFLogger/Logger.h>
#include <NFLogger/StdStreamSink.h>

using namespace nativeformat::logger;

// Define our test message
class TestMessage {
public:
  TestMessage(int code, std::string context)
      : _code(code), _context(std::move(context)) {}
  virtual ~TestMessage() {}
  friend std::ostream &operator<<(std::ostream &os, const TestMessage &tm);

  const int _code;
  const std::string _context;
};

std::ostream &operator<<(std::ostream &os, const TestMessage &tm) {
  os << tm._code << ": " << tm._context;
  return os;
}

// Define our log information for the message
template <> class LogInfo<TestMessage> : public LogInfoBase<TestMessage> {
public:
  LogInfo(std::unique_ptr<TestMessage> payload, Severity level,
          std::string domain = DEFAULT_DOMAIN)
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

// Create our logger and add our log sinks to it
Logger<LogInfoHandler<std::string>, LogInfoHandler<TestMessage>> testLogger;
auto stream_sink = std::make_shared<StdStreamSink>(
    "Test stdout", std::shared_ptr<std::ostream>(&outBuf, [](void *) {}),
    std::shared_ptr<std::ostream>(&errBuf, [](void *) {}));
testLogger.addSink<std::string>(stream_sink, LogInfoFormat::STRING);
testLogger.addSink<TestMessage>(stream_sink, LogInfoFormat::JSON);

// Log some information
INFO(testLogger, "Info from default domain");
ERROR(testLogger, "String error");
auto li =
    makeLogInfo<TestMessage>(testDomain, Severity::ERROR, 3, "Doing stuff");
ERROR(testLogger, li);
```

The above will log to the standard output on the console.

## Contributing :mailbox_with_mail:
Contributions are welcomed, have a look at the [CONTRIBUTING.md](CONTRIBUTING.md) document for more information.

## License :memo:
The project is available under the [Apache 2.0](http://www.apache.org/licenses/LICENSE-2.0) license.

### Acknowledgements
- Icon in readme banner is “[log in](https://thenounproject.com/search/?q=logging&i=647414)” by Eagle Eye from the Noun Project.

#### Contributors
* Julia Cox
* David Rubinstein
* Justin Sarma
* Will Sackfield
