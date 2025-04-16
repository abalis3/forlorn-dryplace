#include "ServerLog.h"

double Logger::totalExecTime = 0;

LogOutputter::LogOutputter(double timestamp) {
    std::cout << timestamp << ": ";
}

LogOutputter::~LogOutputter() {
    std::cout << std::endl;
}

LogOutputter Logger::log() {
    return LogOutputter(totalExecTime);
}

void Logger::update(double elapsedSecs) {
    totalExecTime += elapsedSecs;
}