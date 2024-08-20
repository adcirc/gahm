//
// Created by Zach Cobell on 8/19/24.
//

#ifndef GAHM_LOG_H
#define GAHM_LOG_H

#include <memory>

#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include "quill/sinks/ConsoleSink.h"

namespace Gahm::Logging {

/**
 * @brief A class that wraps the Quill logging library. Singleton class.
 */
class Log {
 public:
  static auto instance() -> quill::Logger* {
    static const Log instance;
    return instance.m_logger.get();
  }

 private:
  Log() : m_logger(nullptr) {
    const quill::BackendOptions backend_options;
    quill::Backend::start(backend_options);

    auto console_sink =
        quill::Frontend ::create_or_get_sink<quill::ConsoleSink>("console");
    m_logger =
        std::unique_ptr<quill::Logger>(quill::Frontend::create_or_get_logger(
            "gahm_console_logger", std::move(console_sink)));
    m_logger->set_log_level(quill::LogLevel::TraceL3);
  }

  std::unique_ptr<quill::Logger> m_logger;
};

}  // namespace Gahm::Logging

#endif  // GAHM_LOG_H
