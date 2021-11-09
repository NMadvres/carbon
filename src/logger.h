#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <string>
#include <fstream>
#include <sstream>

namespace logger
{
enum class level
{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

class base_logger;
class file_logger;
class console_logger;
class log_stream;

class base_logger
{
private:
    bool m_enable;

public:
    base_logger():
        m_enable(true)
    {}

    virtual ~base_logger() = default;

    virtual void output(level lv, std::string &&msg) = 0;

    log_stream operator()(level lv);

    void enable()
    {
        m_enable = true;
    }

    void disable()
    {
        m_enable = false;
    }

    bool is_enabled()
    {
        return m_enable;
    }
};

class log_stream: public std::ostringstream
{
public:
    log_stream(base_logger &logger, level lv):
        _logger(logger), _lv(lv)
    {}

    log_stream(const log_stream &ls):
        _logger(ls._logger)
    {}

    ~log_stream();

private:
    base_logger &_logger;
    level _lv;
};

class console_logger: public base_logger
{
public:
    using base_logger::base_logger;

    void output(level lv, std::string &&msg);
};

class file_logger: public base_logger
{
public:
    file_logger(std::string filename);

    file_logger(const file_logger &) = delete;

    file_logger(file_logger &&) = delete;

    ~file_logger();

    void output(level lv, std::string &&msg);

private:
    std::ofstream _file;
};

}; // namespace logger

#endif // __LOGGER_H__
