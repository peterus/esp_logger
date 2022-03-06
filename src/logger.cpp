#include "logger.h"

namespace logging {
Logger::Logger() : _serial(&Serial), _level(LoggerLevel::LOGGER_LEVEL_DEBUG), _printIsNewline(true), _isSyslogSet(false) {
}

Logger::Logger(LoggerLevel level) : _serial(&Serial), _level(level), _printIsNewline(true), _isSyslogSet(false) {
}

Logger::Logger(Stream *serial) : _serial(serial), _level(LoggerLevel::LOGGER_LEVEL_DEBUG), _printIsNewline(true), _isSyslogSet(false) {
}

Logger::Logger(Stream *serial, LoggerLevel level) : _serial(serial), _level(level), _printIsNewline(true), _isSyslogSet(false) {
}

Logger::~Logger() {
}

// cppcheck-suppress unusedFunction
void Logger::setSerial(Stream *serial) {
  _serial = serial;
}

// cppcheck-suppress unusedFunction
void Logger::setDebugLevel(LoggerLevel level) {
  _level = level;
}

// cppcheck-suppress unusedFunction
void Logger::setSyslogServer(const String &server, unsigned int port, const String &hostname) {
  _syslogServer   = server;
  _syslogPort     = port;
  _syslogHostname = hostname;
  _isSyslogSet    = true;
}

// cppcheck-suppress unusedFunction
void Logger::setSyslogServer(IPAddress ip, unsigned int port, const String &hostname) {
  _syslogIp       = ip;
  _syslogPort     = port;
  _syslogHostname = hostname;
  _isSyslogSet    = true;
}

// cppcheck-suppress unusedFunction
void Logger::print(LoggerLevel level, const String &module, const String &text) {
  if (level <= _level) {
    _serial->print(level.getLineColor());
    printHeader(level, module, false);
    _serial->print(text);
    _serial->print(level.resetColor());
  }
  if (_isSyslogSet) {
    syslogLog(level, module, text);
  }
}

// cppcheck-suppress unusedFunction
void Logger::println(LoggerLevel level, const String &module, const String &text) {
  if (level <= _level) {
    _serial->print(level.getLineColor());
    printHeader(level, module, true);
    _serial->print(text);
    _serial->println(level.resetColor());
  }
  if (_isSyslogSet) {
    syslogLog(level, module, text);
  }
}

void Logger::printHeader(LoggerLevel level, const String &module, bool isln) {
  if (_printIsNewline) {
    _serial->print("[");
    _serial->print(level.toString());
    _serial->print("][");
    _serial->print(module);
    _serial->print("] ");
    if (!isln) {
      _printIsNewline = false;
    }
  } else {
    _printIsNewline = isln;
  }
}

void Logger::syslogLog(LoggerLevel level, const String &module, const String &text) {
  int result;
  if (_syslogIp == INADDR_NONE) {
    result = _syslogUdp.beginPacket(_syslogServer.c_str(), _syslogPort);
  } else {
    result = _syslogUdp.beginPacket(_syslogIp, _syslogPort);
  }

  if (result != 1) {
    return;
  }

#define LOG_KERN (0 << 3) /* kernel messages */

  _syslogUdp.print('<');
  _syslogUdp.print(LOG_KERN | level.getValue());
  _syslogUdp.print(">1 - ");
  _syslogUdp.print(_syslogHostname);
  _syslogUdp.print(' ');
  _syslogUdp.print(module);
  _syslogUdp.print(" - - - \xEF\xBB\xBF");
  _syslogUdp.print(text);
  _syslogUdp.endPacket();
}
} // namespace logging
