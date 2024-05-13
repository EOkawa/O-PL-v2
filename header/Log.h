#ifndef LOG_H
#define LOG_H
#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <windows.h>
#include <iostream>

#include "fileapi.h"

#define LOG systemLog::get().write // Shorten the logging call

class systemLog {
	public:
		static systemLog& get() {
			static systemLog instance;
			return instance;
		}	
		int init(bool enable);
		void write(std::string message);
		void uninit();

private:
		systemLog();
		~systemLog();

		std::ofstream logFile;		// log file
		std::mutex mtx_log;			// mutex
		bool enabled;				// logging is enabled/disabled
		std::string fileName;

		std::string getCurrentDateTime();
		void getFileName();
};
#endif