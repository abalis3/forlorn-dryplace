#ifndef FD__UTIL_H
#define FD__UTIL_H

#include <string>

/* Macros to define what OS we are building on */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
   #define COMPILING_ON_WINDOWS 1
#elif __APPLE__
   #define COMPILING_ON_OSX 1
#elif __linux__
   #define COMPILING_ON_LINUX 1
#endif

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

    /*
     * Forms an absolute filepath given the given relative path.
     * The relative path will be added within the dedicated
     * preferences storage path in the persistent data folder
     * for the game on a given system
     */
    static std::string formPrefsPath(std::string relativePath);

 private:

    /* Holds the absolute path to the directory where the executable resides */
    static std::string exeDir;

#ifndef COMPILING_ON_WINDOWS
    /* Holds the path to the home directory (on Linux or MAC) */
    static std::string homeDir;
#endif

    /* 
     * Forms the given filepath relative to the persistent data folder
     * for the game on a given system.
     */
    static std::string formPersistDataPath(std::string relativePath);

    /* Creates all the directories that don't exist in the given filepath */
    static void initDirectoriesForPath(std::string path);
};

#endif