#include "Util.h"

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