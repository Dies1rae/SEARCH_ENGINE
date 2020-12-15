#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include <ostream>

#define PROFILE_CONCAT_INTERNAL(X, Y) X ## Y
#define PROFILE_CONCAT(X, Y) PROFILE_CONCAT_INTERNAL(X, Y)
#define UNIQUE_VAR_NAME_PROFILE PROFILE_CONCAT(profileGuard, __LINE__)
#define LOG_DURATION(x) LogDuration UNIQUE_VAR_NAME_PROFILE(x) 
#define LOG_DURATION_STREAM(x, y) LogDuration UNIQUE_VAR_NAME_PROFILE(x, y)

class LogDuration {
public:
    using Clock = std::chrono::steady_clock;

    LogDuration(const std::string& N, std::ostream& out = std::cerr) :name_of_the_game(N), out_(out) {}

    LogDuration(const std::string_view& N, std::ostream& out = std::cerr) :name_of_the_game(N), out_(out) {}

    ~LogDuration() {
        using namespace std::chrono;
        using namespace std::literals;

        const auto end_time = Clock::now();
        const auto dur = end_time - this->start_time_;
        this->out_ << this->name_of_the_game << ": "s << duration_cast<milliseconds>(dur).count() << " ms"s << std::endl;
    }

private:
    const std::string name_of_the_game;
    std::ostream& out_;
    const Clock::time_point start_time_ = Clock::now();
};