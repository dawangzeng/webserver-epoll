#ifndef __LOGGER_H__
#define __LOGGER_H__
#include<iostream>
#include<fstream>
using namespace std;
typedef enum {
    INFO,
    WARNING,
    ERROR
}LOG_LEVEL;
class Log 
{
    private:
        fstream *fstr;
    public:
        Log();
        ~Log();
        void LogInfo(LOG_LEVEL level, ...);
        void LogWriteToFile();
}

#endif