#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
public:
    Timer();

    void start();
    void stop();
    void reset();

    double elapsedMilliseconds() const;
    double elapsedSeconds() const;

    bool isRunning() const;

private:
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point endTime_;
    bool running_;
};

#endif