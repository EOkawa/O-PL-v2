#include "tiffImage.h"

int MLTiffW::open(const std::string& path) {
	this->tiff = tifio::TIFFOpen(path.c_str(), "w");
	if (!this->tiff) return -1;
	this->path = path;
	return 1;
}

int MLTiff::close() {
	if (this->tiff) TIFFClose(this->tiff);
	this->tiff = nullptr;
	return 1;
}

DataType MLTiff::dirInfoToType(DirInfo* info) {
	if (info->sampleFormat == 1) {
		if (info->bitsPerSample == 1) return DataType::Binary;
		else if (info->bitsPerSample == 8) {
			if (info->height == 1) return DataType::String;
			else return DataType::U8;
		}
		else if (info->bitsPerSample == 16) {
			if (info->xRes == 1 || info->xRes > 1.0001 || info->xRes < 0.9999) return DataType::U16;
			else return DataType::F16;
		}
		else return DataType::None;
	}
	else if (info->sampleFormat == 3) {
		if (info->bitsPerSample == 32) return DataType::F32;
		else return DataType::None;
	}
	return DataType::None;
}

MLTiffW::MLTiffW() {
	this->tiff = nullptr;
	this->dirCount = 0;
}

MLTiffW::~MLTiffW() {
	this->close();
}

int MLTiffW::writeU16(const std::string& name, uint16_t* data, int width, int height) {
	DirInfo info;
	info.name = name;
	int e = this->setName(info.name);

	if (e == 1)
	{
		size_t byteLength = (size_t)width * (size_t)height * sizeof(uint16_t);
		info.width = width;
		info.height = height;
		info.bitsPerSample = 16;
		info.samplesPerPixel = 1;
		info.sampleFormat = 1;
		info.xRes = 1;
		info.yRes = 1;
		e = this->writeDirectory(&info, data, byteLength);
	}
	return e;
}

int MLTiffW::writeTIFF(const std::string& path, uint16_t* data, int rows, int columns)
{
	int e = this->open(path);
	if (e == 1)
	{
		e = this->writeU16(path, data, columns, rows);
		this->close();
	}
	return e;
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int MLTiffW::writeDirectory(DirInfo* info, void* data, size_t bytelength) {
	TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, info->width);
	TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, info->height);
	TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, info->bitsPerSample);
	TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, info->sampleFormat);
	TIFFSetField(tiff, TIFFTAG_COMPRESSION, 1);
	TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, 1);
	TIFFSetField(tiff, TIFFTAG_FILLORDER, 1);
	TIFFSetField(tiff, TIFFTAG_ORIENTATION, 1);
	TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, info->samplesPerPixel);
	TIFFSetField(tiff, TIFFTAG_XRESOLUTION, info->xRes);
	TIFFSetField(tiff, TIFFTAG_YRESOLUTION, info->yRes);
	TIFFSetField(tiff, TIFFTAG_RESOLUTIONUNIT, 1);
	TIFFSetField(tiff, TIFFTAG_PAGENAME, info->name.c_str());
	TIFFWriteEncodedStrip(tiff, 0, data, bytelength);
	TIFFWriteDirectory(tiff);
	this->dirCount++;
	return 1;
}

int MLTiffW::setName(std::string& name) {
	if (name.length() == 0) {
		if (this->dirCount == 0) {
			name = "main";
		}
		else return -1;
	}
	return 1;
}

MLTiffR::MLTiffR() {
	this->dirCount = 0;
}

MLTiffR::~MLTiffR() {
	this->close();
}

int MLTiffR::open(const std::string& path) {
	this->tiff = tifio::TIFFOpen(path.c_str(), "r");
	if (!this->tiff) return -1;
	this->path = path;
	do {
		this->dirCount++;
	} while (TIFFReadDirectory(this->tiff));
	TIFFSetDirectory(this->tiff, 0);
	return 1;
}

int MLTiffR::readU16(int dir, uint16_t* data) 
{
	DirInfo info;
	int e = this->readPre(dir, &info, DataType::U16);
/*	if (e == 1) {
		*width = info.width;
		*height = info.height;
		out.assign(*width * *height, 0);
	}*/
	if (e == 1) e = this->readU16Core(data);
	return e;
}

int MLTiffR::getDirectory(DirInfo* info, int dir) {
	int e = 1;
	e = this->setDir(dir);
	if (e == 1) e = this->getDirInfo(info);
	return e;
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int MLTiffR::readU16Core(uint16_t* out) {
	uint32_t strips = TIFFNumberOfStrips(tiff);
	size_t stripLen = TIFFStripSize(tiff) / sizeof(uint16_t);
	for (uint32_t i = 0; i < strips; i++) {
		TIFFReadEncodedStrip(tiff, i, &out[i * stripLen], -1);
	}
	return 1;
}

int MLTiffR::readPre(int dir, DirInfo* info, DataType typeReq) {
	int e = this->setDir(dir);
	if (e == 1) e = this->getDirInfo(info);
	if (e == 1 && this->dirInfoToType(info) != typeReq)
		e = -2;
	return e;
}

int MLTiffR::setDir(int dir) {
	if (dir >= dirCount || dir < 0) return -1;
	TIFFSetDirectory(this->tiff, (uint16_t)dir);
	return 1;
}

int MLTiffR::getDirInfo(DirInfo* info) {
	char* name;
	if (!TIFFGetField(this->tiff, TIFFTAG_PAGENAME, &name)) info->name = "";
	else info->name = name;
	TIFFGetField(this->tiff, TIFFTAG_IMAGEWIDTH, &(info->width));
	TIFFGetField(this->tiff, TIFFTAG_IMAGELENGTH, &(info->height));
	TIFFGetField(this->tiff, TIFFTAG_BITSPERSAMPLE, &(info->bitsPerSample));
	TIFFGetField(this->tiff, TIFFTAG_SAMPLESPERPIXEL, &(info->samplesPerPixel));
	if (!TIFFGetField(this->tiff, TIFFTAG_SAMPLEFORMAT, &(info->sampleFormat))) info->sampleFormat = 1;
	TIFFGetField(this->tiff, TIFFTAG_XRESOLUTION, &(info->xRes));
	TIFFGetField(this->tiff, TIFFTAG_YRESOLUTION, &(info->yRes));
	return 1;
}