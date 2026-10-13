#include "timer.h"

Timer::Timer()
    : startTime_(),
      endTime_(),
      running_(false) {
}

void Timer::start() {
    startTime_ = std::chrono::steady_clock::now();
    endTime_ = startTime_;
    running_ = true;
}

void Timer::stop() {
    if (!running_) {
        return;
    }

    endTime_ = std::chrono::steady_clock::now();
    running_ = false;
}

void Timer::reset() {
    startTime_ = std::chrono::steady_clock::time_point();
    endTime_ = std::chrono::steady_clock::time_point();
    running_ = false;
}

double Timer::elapsedMilliseconds() const {
    if (running_) {
        const auto now = std::chrono::steady_clock::now();

        return std::chrono::duration<double, std::milli>(
            now - startTime_
        ).count();
    }

    return std::chrono::duration<double, std::milli>(
        endTime_ - startTime_
    ).count();
}

double Timer::elapsedSeconds() const {
    return elapsedMilliseconds() / 1000.0;
}

bool Timer::isRunning() const {
    return running_;
}