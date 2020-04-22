#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

enum level_enum
{
    trace = 0,
    debug,
    info,
    warn,
    err,
    critical,
    off,
    n_levels
};

class tx_env
{
private:
    /* data */
private:
    tx_env(/* args */)
    {
        //initialize log.
        spdlog::set_level(spdlog::level::info);
        auto my_logger = spdlog::basic_logger_mt("file_logger", "logs/basic-log.txt");
        spdlog::set_default_logger(my_logger);
        spdlog::flush_every(std::chrono::seconds(3));
    }
    ~tx_env() = default;

public:
    inline static tx_env &instance()
    {
        static tx_env _instance;
        return _instance;
    }

public:
    template <typename... Args>
    void log(const char *file, int line, const char *funcname, level_enum level, const char *fmt, Args... args)
    {
        char buf__[1024]={0};
        snprintf(buf__, sizeof(buf__), fmt, args...);
        switch (level)
        {
        case err:
            spdlog::default_logger_raw()->log(spdlog::source_loc{file, line, static_cast<const char *>(funcname)}, spdlog::level::err, buf__, args...);
            break;
        case warn:
            spdlog::default_logger_raw()->log(spdlog::source_loc{file, line, static_cast<const char *>(funcname)}, spdlog::level::warn, buf__, args...);
            break;
        case info:
            spdlog::default_logger_raw()->log(spdlog::source_loc{file, line, static_cast<const char *>(funcname)}, spdlog::level::info, buf__, args...);
            break;
        case trace:
            spdlog::default_logger_raw()->log(spdlog::source_loc{file, line, static_cast<const char *>(funcname)}, spdlog::level::trace, buf__, args...);
            break;
        case critical:
            spdlog::default_logger_raw()->log(spdlog::source_loc{file, line, static_cast<const char *>(funcname)}, spdlog::level::critical, buf__, args...);
            break;

        default:
            break;
        }
    }

public:
    void InitDevInterface()
    {
    }
};

#define loginfo(fmt, args...) tx_env::instance().log(__FILE__, __LINE__, __FUNCTION__, info, fmt, ##args)
#define logerr(fmt, args...) tx_env::instance().log(__FILE__, __LINE__, __FUNCTION__, err, fmt, ##args)
#define logtrace(fmt, args...) tx_env::instance().log(__FILE__, __LINE__, __FUNCTION__, trace, fmt, ##args)
#define logcritical(fmt, args...) tx_env::instance().log(__FILE__, __LINE__, __FUNCTION__, critical, fmt, ##args)
#define logwarn(fmt, args...) tx_env::instance().log(__FILE__, __LINE__, __FUNCTION__, warn, fmt, ##args)
