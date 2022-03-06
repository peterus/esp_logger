#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <Arduino.h>
#include <WiFiUdp.h>

#include "logger_level.h"

namespace logging {

class Logger {
public:
  Logger();
  Logger(LoggerLevel level);
  Logger(Stream *serial);
  Logger(Stream *serial, LoggerLevel level);
  ~Logger();

  void setSerial(Stream *serial);
  void setDebugLevel(LoggerLevel level);

  // syslog config
  void setSyslogServer(const String &server, unsigned int port, const String &hostname);
  void setSyslogServer(IPAddress ip, unsigned int port, const String &hostname);

  void print(LoggerLevel level, const String &module, const String &text);
  void println(LoggerLevel level, const String &module, const String &text);

private:
  Stream *    _serial;
  LoggerLevel _level;
  bool        _printIsNewline;

  void printHeader(LoggerLevel level, const String &module, bool isln);

  // syslog members
  bool      _isSyslogSet;
  WiFiUDP   _syslogUdp;
  String    _syslogServer;
  IPAddress _syslogIp;
  int       _syslogPort;
  String    _syslogHostname;

  void syslogLog(LoggerLevel level, const String &module, const String &text);
};
} // namespace logging

#endif
