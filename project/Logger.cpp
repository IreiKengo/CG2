#include "Logger.h"
#include <Windows.h>
#include <fstream>






namespace Logger
{

	void Log(const std::string& message)
	{
		OutputDebugStringA(message.c_str());
	}

	void Log(std::ostream& os, const std::string& message)
	{
		os << message << std::endl;

		OutputDebugStringA(message.c_str());
	}

}
