#ifndef FD__SERVERLOG_H
#define FD__SERVERLOG_H

#include <string>
#include <iostream>

/* Returned as a helper class to actually stream a single log to stdout with prefix / suffix */
class LogOutputter {
 public:

    /* Destructor which is called at the end of the log line. Appends endl */
    ~LogOutputter();

    /* Output the given data via stdout */
    template<typename T> LogOutputter& operator<<(T const &data) {
        std::cout << data;
        return *this;
    }

 private:
    /* Private constructor/destructor. Only to be created by Logger when logs are being output */
    LogOutputter(double timestamp);

    friend class Logger;
};

class Logger {

 public:

    /* Log the given string to the console (this function prepends a timestamp) */
    static LogOutputter log();

    /* Update the logger's stage during each loop of the main thread */
    static void update(double elapsedSecs);

private:

    /* Holds the current timestamp in seconds */
    static double totalExecTime;
};

#endif