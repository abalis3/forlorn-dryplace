#include "Util.h"

#include <cstdlib>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

static const char PREFS_DIRECTORY_NAME[] = "preferences";

/* Use backslash for windows, forward slash for other OS's */
#if COMPILING_ON_WINDOWS
static const char DIRECTORY_DELIM = '\\';
#else
static const char DIRECTORY_DELIM = '/';
std::string Util::homeDir;
#endif

std::string Util::exeDir;

void Util::registerRunArgs(int argc, char *argv[])
{    
    std::string fullPath = argv[0];

    // Find the position of the last slash in the path
    int pos = 0;
    int lastSlash = -1;
    while (fullPath[pos] != '\0') {
        if (fullPath[pos] == '/' || fullPath[pos] == '\\') {
            lastSlash = pos;
        }
        pos++;
    }

    // Save the substring up to and excluding the last slash
    exeDir = fullPath.substr(0, lastSlash);
}

std::string Util::formResourcePath(std::string resourceName)
{
#if OSX_RELEASE_BUILD
    return exeDir + "/res/" + resourceName;
#else
    return "res/" + resourceName;
#endif
}

std::string Util::formPrefsPath(std::string relativePath)
{
    std::string fullPrefsPath = formPersistDataPath(PREFS_DIRECTORY_NAME);
    initDirectoriesForPath(fullPrefsPath);
    return fullPrefsPath + DIRECTORY_DELIM + relativePath;
}

std::string Util::formPersistDataPath(std::string relativePath)
{
#if COMPILING_ON_WINDOWS
    /* Get to '%APPDATA%/LocalLow/abalis3/forbidden-desert/' */
    std::string path = std::string(std::getenv("LOCALAPPDATA"));
    path += "Low\\abalis3\\forbidden-desert\\";
    path += relativePath;
    return path;
#else
    /* Compiling on Linux or OSX */

    /* Get the path to the home (~) directory */
    if (homeDir == "") {
        homeDir = std::string(std::getenv("HOME"));
    }

#if COMPILING_ON_OSX
    /* Get to '~/Library/Application Support/abalis3/forbidden-desert/' */
    return homeDir + "/Library/Application Support/abalis3/forbidden-desert/";
#else /* Compiling on Linux */
    /* Get to '~/.abalis3/forbidden-desert/' */
    return homeDir + "/.abalis3/forbidden-desert/";
#endif

#endif

}

void Util::initDirectoriesForPath(std::string path)
{
    if (!std::experimental::filesystem::exists(path)) {
        std::experimental::filesystem::create_directories(path);
    }
}