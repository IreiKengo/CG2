#include "Logger.h"
#include <Windows.h>
#include <fstream>






namespace Logger
{

	std::wstring ToWString(const std::string& str)
	{
		if (str.empty()) return L"";

		int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
		std::wstring wstr(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
		return wstr;
	}


	void Log(const std::string& message)
	{
		OutputDebugStringW(ToWString(message).c_str());
	}

	void Log(std::ostream& os, const std::string& message)
	{
		os << message << std::endl;

		OutputDebugStringW(ToWString(message).c_str());
	}


}
