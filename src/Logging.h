#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <string>

class Logging {
 public:
  Logging() = default;

  static void throwError(const std::string &s);
  static void throwError(const std::string &s, const char *file, int line);

  static void logError(const std::string &s,
                       const std::string &heading = std::string());
  static void warning(const std::string &s,
                      const std::string &heading = std::string());
  static void log(const std::string &s,
                  const std::string &heading = std::string());

 private:
  static void printMessage(const std::string &header,
                           const std::string &message);
  static void printErrorMessage(const std::string &header,
                                const std::string &message);
};

#define gahm_throw_exception(arg) Logging::throwError(arg, __FILE__, __LINE__)

#endif  // LOGGING_H
