#include "../header/logger.h"
#include <cstring>

Logger::Logger()
{

}

Logger::~Logger()
{

}

void Logger::Print(const char *tag, const char *content)
{
    char str[66];
    time_t td = time(NULL);
    tm *current_time = NULL;
    current_time = localtime(&td);
    strftime(str, 66, "%F-%T", current_time);
    std::cout << GREEN << str << SRC << "[" << tag << "]:" << content << std::endl;
    
}

void Logger::Print(const char *tag, const char *content, const char  *variable)
{
    char str[66];
    time_t td = time(NULL);
    tm *current_time = NULL;
    current_time = localtime(&td);
    strftime(str, 66, "%F-%T", current_time);
    std::cout << GREEN << str << SRC << "[" << tag << "]:" << content  << variable << std::endl;
    
}

void Logger::Print(const char *tag, const char *content, std::string variable)
{
    char str[66];
    time_t td = time(NULL);
    tm *current_time = NULL;
    current_time = localtime(&td);
    strftime(str, 66, "%F-%T", current_time);
    std::cout << GREEN << str << SRC << "[" << tag << "]:" << content << variable << std::endl;
    
}

//打印错误信息
void Logger::PrintError(const char *tag, const char *content)
{
    char str[66];
    time_t td = time(NULL);
    tm *current_time = NULL;
    current_time = localtime(&td);
    strftime(str, 66, "%F-%T", current_time);
    std::cout << RED << str << "[" << tag << "]:" << content << SRC << std::endl;
}

void Logger::InputVariable(char *variable)
{
    char str[66];
    time_t td = time(NULL);
    tm *current_time = NULL;
    current_time = localtime(&td);
    strftime(str, 66, "%F-%T", current_time);
    std::cout << GREEN << str << SRC << "[please input]:";
    
    std::cin >> variable;
}

void Logger::InputVariable(const char *tag, const char *content, int64_t *variable)
{
    char str[66];
    time_t td = time(NULL);
    tm *current_time = NULL;
    current_time = localtime(&td);
    strftime(str, 66, "%F-%T", current_time);
    std::cout << GREEN << str << SRC << "[" << tag << "]:" << content ;
    
    std::cin >> *variable ;
}

void Logger::InputVariable(const char *tag, const char *content, char *variable)
{
    char str[66];
    time_t td = time(NULL);
    tm *current_time = NULL;
    current_time = localtime(&td);
    strftime(str, 66, "%F-%T", current_time);
    std::cout << GREEN << str << SRC << "[" << tag << "]:" << content ;
    
    if(0 == strcmp(content, "please input password : ") || 0 == strcmp(content, "input password again  : "))
    {
        std::cout << HIDE ;
        std::cin >> variable ;
        std::cout << SRC;
    }
    else
    {
        std::cin >> variable;
    }
    
}