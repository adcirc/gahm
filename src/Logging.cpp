#include "logging.h"

static const std::string c_errorHeading("[GAHM ERROR]: ");
static const std::string c_warningHeading("[GAHM WARNING]: ");
static const std::string c_logHeading("[GAHM INFO]: ");

/**
 * @brief Throws a runtime error
 * @param[in] s error description
 */
void Logging::throwError(const std::string &s) {
  throw std::runtime_error(c_errorHeading + s);
}

/**
 * @brief Throws an error with file name and line number
 * @param[in] s error description
 * @param[in] file file name where error occured
 * @param[in] line line number where error occured
 */
void Logging::throwError(const std::string &s, const char *file, int line) {
  throw std::runtime_error(c_errorHeading + s + " at " + file + ", line " +
                           std::to_string(line));
}

/**
 * @brief Logs an error message to the screen
 * @param s error message
 */
void Logging::logError(const std::string &s, const std::string &heading) {
  std::string header = c_errorHeading;
  if (heading != std::string()) {
    header = heading;
  }
  Logging::printErrorMessage(heading, s);
}

/**
 * @brief Logs a warning message
 * @param[in] s warning message
 */
void Logging::warning(const std::string &s, const std::string &heading) {
  std::string header = c_warningHeading;
  if (heading != std::string()) {
    header = heading;
  }
  Logging::printMessage(header, s);
}

/**
 * @brief Informational log message
 * @param s message string
 */
void Logging::log(const std::string &s, const std::string &heading) {
  std::string header = c_logHeading;
  if (heading != std::string()) {
    header = heading;
  }
  Logging::printMessage(header, s);
}

/**
 * @brief Prints a message to screen
 * @param header custom header
 * @param message log message
 */
void Logging::printMessage(const std::string &header,
                           const std::string &message) {
  std::cout << header << message << std::endl;
}

/**
 * @brief Prints a message to standard error
 * @param header custom header
 * @param message log message
 */
void Logging::printErrorMessage(const std::string &header,
                                const std::string &message) {
  std::cerr << header << message << std::endl;
}
