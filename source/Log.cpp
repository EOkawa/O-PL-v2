#include "log.h"

systemLog::systemLog() 
{
    this->enabled = false;
    this->fileName = "";
}

systemLog::~systemLog() {
}

int systemLog::init(bool enable) 
{
    if (enable)
    {
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        this->enabled = true;
    }
    return 1;
}

void systemLog::write(std::string message) {
    if (this->enabled) std::cout << getCurrentDateTime() << "\t" << message << "\n";
}

std::string systemLog::getCurrentDateTime()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;

    oss << std::put_time(&tm, "%Y/%m/%d %H:%M:%S");
    return oss.str();
}

void systemLog::getFileName()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;

    oss << std::put_time(&tm, "%Y%m%dT%H%M%S");
    this->fileName = "log//" + oss.str() + " log.txt";

    OutputDebugString(("[Log File Name] "+ this->fileName + '\n').c_str());
}

void systemLog::uninit() {
}