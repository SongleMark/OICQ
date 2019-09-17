#ifndef LOGGER_H
#define LOGGER_H

#include <time.h>
#include <stdint.h>
#include <iostream>

#define LOG   Logger::Print
#define LOGI  Logger::InputVariable
#define LOGE  Logger::PrintError 
#define GREEN "\e[32m"
#define RED   "\e[31m"
#define SRC   "\e[0m"
#define HIDE  "\e[8m"

class Logger
{
public:
    explicit Logger();
    virtual ~Logger();
    static void Print(const char *tag, const char *content);
    static void Print(const char *tag, const char *content, const char *variable);
    static void Print(const char *tag, const char *content, std::string variable);
    template <typename T>
    static void Print(const char *tag, const char *content, T variable)
    {
        char str[66];
        time_t td = time(NULL);
        tm *current_time = NULL;
        current_time = localtime(&td);
        strftime(str, 66, "%F-%T", current_time);
        std::cout << GREEN << str << SRC << "[" << tag << "]:" << content << variable << std::endl;
        //std::cout << current_time->tm_year + 1900 << "-" << current_time->tm_mon + 1 << "-" << current_time->tm_mday<< "-" <<
                  //current_time->tm_hour << ":" << current_time->tm_min << ":" << current_time->tm_sec << "[" << tag << "]:" << 
                  //content << variable << std::endl;
    }
    static void PrintError(const char *tag, const char *content);

    static void InputVariable(char *variable);
    static void InputVariable(const char *tag, const char *content, int64_t *variable);
    static void InputVariable(const char *tag, const char *content, char *variable);
    template <typename T>
    static void InputVariable(T *variable)
    {
        std::cin >> *variable ;
    }
    
};

#endif //logger.h 打印日志文件