#include "logger.h"
#include <map>

namespace logger
{
static const std::map<level, const std::string> level_str = {
    {level::DEBUG, "DEBUG"},
    {level::INFO, "INFO"},
    {level::WARNING, "WARNING"},
    {level::ERROR, "ERROR"},
    {level::FATAL, "FATAL"}};

log_stream::~log_stream()
{
    _logger.output(_lv, std::move(this->str()));
}

log_stream base_logger::operator()(level lv)
{
    return log_stream(*this, lv);
}

void console_logger::output(level lv, std::string &&msg)
{
}

file_logger::file_logger(std::string filename)
{
    _file.open(filename,
               std::fstream::out | std::fstream::app | std::fstream::ate);
}

file_logger::~file_logger()
{
    _file.flush();
    _file.close();
}

void file_logger::output(level lv, std::string &&msg)
{
    auto str_lv = level_str.find(lv)->second;

    _file << "[" << str_lv << "]"
          << "\t" << msg << std::endl;
}

}; // namespace logger
