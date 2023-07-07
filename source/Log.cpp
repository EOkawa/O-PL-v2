#include "log.h"

systemLog::systemLog() {
}

systemLog::~systemLog() {
}

int systemLog::init(bool enable) {
    this->enabled = enable;

    if (enable)
    {
        // Create log folder
        std::string path = "log";
        const int n = path.length();
        char* char_path = new char[n + (uint8_t)1]();

        strcpy(char_path, path.c_str());

        if (GetFileAttributes(char_path) == INVALID_FILE_ATTRIBUTES)
        {
            if (!CreateDirectory(char_path, NULL))
            {
                if (GetLastError() == ERROR_ALREADY_EXISTS) {
                    return -1;	// directory already exists
                }
                else {
                    return -2;	// creation failed due to some other reason
                }
            }
        }

        cout << "Creating log file";

        delete[] char_path;
        this->write("Creating Log file...");
    }

    return 1;
}

void systemLog::write(std::string message)
{
    if (this->enabled)
    {
        mtx_log.lock();
        logFile.open("log/log.txt", std::ios_base::app);
        logFile << getCurrentDateTime() << "\t" << message << "\n";
        logFile.close();
        mtx_log.unlock();
    }
}

std::string systemLog::getCurrentDateTime()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;

    oss << std::put_time(&tm, "%Y/%m/%d %H:%M:%S");
    return oss.str();
}

void systemLog::uninit() {
}