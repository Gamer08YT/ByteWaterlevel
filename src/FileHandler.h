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
    static String readFile(const char* path);
    static bool loadConfig();
    static void saveFile(const char* str, const String& string);
};


#endif //FILEHANDLER_H
