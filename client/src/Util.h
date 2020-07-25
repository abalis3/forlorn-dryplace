#ifndef FD__UTIL_H
#define FD__UTIL_H

#include <string>

/*
 * Class holding utility functions that could be
 * used throughout the game's code
 */
class Util {
 public:

    /*
     * Extract necessary info from runtime arguments -
     * namely the path to the executable
     */
    static void registerRunArgs(int argc, char *argv[]);

    /*
     * Create resource path to prepend to the path within
     * the "res/" directory. On windows this just prepends "res/" to the
     * beginning since res/ is assumed to be in the same directory
     * as the exe. On mac prepend with an absolute path to the resource
     * folder ("/.../res/")
     */
    static std::string formResourcePath(std::string resourceName);

 private:

    /* Holds the absolute path to the directory where the executable resides */
    static std::string exeDir; 
};

#endif