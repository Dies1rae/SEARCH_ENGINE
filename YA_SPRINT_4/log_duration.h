#pragma once
#include <chrono>
#include <iostream>
#include <string>
#include <ostream>
#define PROFILE_CONCAT_INTERNAL(X, Y) X ## Y
#define PROFILE_CONCAT(X, Y) PROFILE_CONCAT_INTERNAL(X, Y)
#define UNIQUE_VAR_NAME_PROFILE PROFILE_CONCAT(profileGuard, __LINE__)
#define LOG_DURATION(x) LogDuration UNIQUE_VAR_NAME_PROFILE(x)
#define LOG_DURATION_STREAM(x, o) LogDuration UNIQUE_VAR_NAME_PROFILE(x,o)

class LogDuration {
public:
    using Clock = std::chrono::steady_clock;

    LogDuration(std::string N, std::ostream& O = std::cerr) : name_of_the_game_(N), out_(O) {}

    ~LogDuration() {
        using namespace std::chrono;
        using namespace std::literals;

        const auto end_time = Clock::now();
        const auto dur = end_time - start_time_;
        std::cerr << this->name_of_the_game_ << ": "s << duration_cast<milliseconds>(dur).count() << " ms"s << std::endl;
    }

private:
    std::ostream& out_;
    const std::string name_of_the_game_;
    const Clock::time_point start_time_ = Clock::now();
};