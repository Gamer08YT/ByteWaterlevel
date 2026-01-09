//
// Created by JanHe on 09.01.2026.
//

#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include <WString.h>


class FileHandler
{
public:
    static void begin();
    String readFile(const char* path);
    static bool loadConfig();
};


#endif //FILEHANDLER_H
