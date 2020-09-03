#pragma once
#include "types.h"
#include <iostream>
#include <string>

#ifdef _DEBUG
#define THROW_EXCEPTION(type, msg)\
throw Exception(type, msg, __FILE__, __LINE__);
#define THROW_EXCEPTION_CODE(type, msg, code)\
throw Exception(type, msg, code, __FILE__, __LINE__);
#else
#define THROW_EXCEPTION(type, msg)\
throw Exception(type, msg);
#define THROW_EXCEPTION_CODE(type, msg, code)\
throw Exception(type, msg, code);
#endif

class Exception : public std::exception
{
protected:
    const char* nErrorLogFile = "errorlog.out";
	std::string info;
    FILE* fErrorLog = nullptr;
public:
	Exception() {}
	Exception(const char* type, const char* msg)
	{
		info += "[" + std::string(type) + "]";
		info += " " + std::string(msg) + "\n";
	}
	Exception(const char* type, const char* msg, const int32 code)
	{
		info += "[" + std::string(type) + "]";
		info += " " + std::string(msg) + "\n";
		info += "[CODE] " + std::to_string(code) + "\n";
	}
	Exception(const char* type, const char* msg, const char* file, const int32 line)
	{
		info += "[" + std::string(type) + "] ";
		info += " " + std::string(msg) + "\n";
		info += "[FILE] " + std::string(file) + "\n";
		info += "[LINE] " + std::to_string(line) + "\n";
	}
	Exception(const char* type, const char* msg, const int32 code, const char* file, const int32 line)
	{
		info += "[" + std::string(type) + "] ";
		info += " " + std::string(msg) + "\n";
		info += "[CODE] " + std::to_string(code) + "\n";
		info += "[FILE] " + std::string(file) + "\n";
		info += "[LINE] " + std::to_string(line) + "\n";
	}
	const char* what() const noexcept
	{
		return info.c_str();
	}

	void logError() const;
	void printError() const;
};

//		const char* point;
//		if ((point = strrchr(fileName, '.')) != NULL) {
//			if (strcmp(point, ".out") != 0) {
//				// does not end with .out
//				printError("Error creating exception: file type must be .out", fileName);
//				return;
//			}
//		}