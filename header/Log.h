#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <windows.h>
#include <iostream>

#include "fileapi.h"

using namespace std;

class systemLog
{
private:
	std::ofstream logFile;		// log file
	std::mutex mtx_log;			// mutex
	bool enabled = false;		// enables/disables logging

	std::string getCurrentDateTime();

public:
	systemLog();
	~systemLog();

	int init(bool enable);
	void write(std::string message);
	void uninit();
};

