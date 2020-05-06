#pragma once

#include <cstdio>
#include <cstring>

#include <array>
#include <string>
#include <sstream>
#include <vector>

#define __FILENAME__ (strrchr("/" __FILE__, '/') + 1)

enum{
    ERROR = 1,
    IMPT = 2,
    INFO = 3,
    DEBUG = 4,
    ALL = 5,
};

constexpr int LOG_LEVEL = ERROR;

template <class... Args>
void log_msg(int level, Args... args){
    if (level > LOG_LEVEL)
        return;

    fprintf(stderr, args...);
    fputc('\n', stderr);
}

#define LOG_MSG(LEVEL, pattern, ...)\
    do{\
        if (LEVEL <= LOG_LEVEL)\
            log_msg(LEVEL, "[" #LEVEL "] %s(%s):%d\t\t" pattern,\
                __FILENAME__, __func__, __LINE__, ##__VA_ARGS__);\
    }while(false);

#define MERROR(...) LOG_MSG(ERROR, ##__VA_ARGS__)
#define MIMPT(...)  LOG_MSG(IMPT, ##__VA_ARGS__)
#define MINFO(...)  LOG_MSG(INFO, ##__VA_ARGS__)
#define MDEBUG(...) LOG_MSG(DEBUG, ##__VA_ARGS__)
#define MALL(...)   LOG_MSG(ALL, ##__VA_ARGS__)

inline std::string to_string(const std::string &s){
    return s;
}

template <typename T1, typename T2>
std::string to_string(const std::pair<T1, T2> &a){
    using ::to_string;
    using std::to_string;
    std::stringstream ss;
    ss << "(" << to_string(a.first) << "," << to_string(a.second) << ")";
    return ss.str();
}

template <typename T>
std::string to_string(const std::vector<T> &a){
    using ::to_string;
    using std::to_string;
    std::stringstream ss;
    ss << "[";
    for (auto &e: a)
        ss << to_string(e) << ",";
    ss << "]";
    return ss.str();
}

template <typename T, size_t N>
std::string to_string(const std::array<T, N> &a){
    return to_string(std::vector<T>(a.begin(), a.end()));
}

template <class... Args>
std::string format(Args... args){
    using ::to_string;
    using std::to_string;
    std::string res;
    int a[] = {0, (res += to_string(args), 0)...};
    static_cast<void>(a);
    return res;
}
