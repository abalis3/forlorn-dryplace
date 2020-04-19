#ifndef FD__UTIL_H
#define FD__UTIL_H

#include <string>

class Util {
 public:
    static void registerRunArgs(int argc, char *argv[]);
    static std::string formResourcePath(std::string resourceName);
 private:
    static std::string exeDir; 
};

#endif