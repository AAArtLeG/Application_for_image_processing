#pragma once

//#include <Eigen/Sparse>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

bool saveToPgm(const std::string& filename, int width, int height, const std::vector<double>& data);

class ImageData {
private:
	int width = 0;
	int height = 0;
	long size = 0;
	std::vector<std::vector<double>> data = std::vector<std::vector<double>>{};
public:

	ImageData() {};
	ImageData(int w, int h, const unsigned char* d, int bytesPerLine, int colorDepth);
	~ImageData() {};
	void setWidth(int w) { width = w; }
	void setHeight(int h) { height = h; }
	int getWidth() { return width; }
	int getHeight() { return height; }
	std::vector<std::vector<double>>& getData() { return data; }
	std::vector<double>& getData(int channelId) { return data[channelId]; }
};
class ImageProcessing {
public:
	ImageProcessing() {};
	~ImageProcessing() {};
};