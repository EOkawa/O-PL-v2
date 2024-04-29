#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace tifio {
	#include "tiffio.h"
}

using namespace std;

struct DirInfo {
	std::string name;
	uint32_t width;
	uint32_t height;
	uint16_t bitsPerSample;
	uint16_t samplesPerPixel;
	uint16_t sampleFormat;
	float xRes;
	float yRes;
};

enum class DataType : uint16_t {
	None,
	Binary,
	U8,
	U16,
	F16,
	F32,
	String
};

class MLTiff
{
public:
	int close();

protected:
	std::string path;
	tifio::TIFF* tiff = nullptr;
	DataType dirInfoToType(DirInfo* info);
	virtual int open(const std::string& path) = 0;
};

class MLTiffW :
	public MLTiff
{
public:
	MLTiffW();
	~MLTiffW();

	int writeTIFF(const std::string& path, uint16_t* data, int rows, int columns);

private:
	int dirCount;

	int open(const std::string& path) override;
	int writeU16(const std::string& name, uint16_t* data, int width, int height);
	int writeDirectory(DirInfo* info, void* data, size_t byteLength);
	int setName(std::string& name);
};

class MLTiffR :
	public MLTiff
{
public:
	MLTiffR();
	~MLTiffR();

	int getDirCount() { return this->dirCount; };
	int open(const std::string& path) override;
	int getDirectory(DirInfo* info, int dir);
	int readU16(int dir, uint16_t* data);

private:
	int dirCount;
	int setDir(int dir);
	int getDirInfo(DirInfo* info);
	int readPre(int dir, DirInfo* info, DataType reqType);
	int readU16Core(uint16_t* out);
};	