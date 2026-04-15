#pragma once

//#include <Eigen/Sparse>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <QImage.h>

using namespace std;

bool saveToPgm(const std::string& filename, int width, int height, const std::vector<double>& data);

class ImageData {
private:
	int width = 0;
	int height = 0;
	long size = 0;
	std::vector<std::vector<double>> data = std::vector<std::vector<double>>{};
	double kernel[5][5] = {
		{0.001951161393699, 0.010659081074241, 0.018722276705751, 0.010659081074241, 0.001951161393699},
		{0.010659081074241, 0.058229939211677, 0.102278707412656, 0.058229939211677, 0.010659081074241},
		{0.018722276705751, 0.102278707412656, 0.189999012510939, 0.102278707412656, 0.018722276705751},
		{0.010659081074241, 0.058229939211677, 0.102278707412656, 0.058229939211677, 0.010659081074241},
		{0.001951161393699, 0.010659081074241, 0.018722276705751, 0.010659081074241, 0.001951161393699}
	};

	void findMinMax(std::vector<double>& arr, double& min, double& max);
	std::vector<std::vector<double>> mirroring(std::vector<double>& arr);
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
	void setData(std::vector<std::vector<double>>& d) { data = d; }
	void getData(int channelId, std::vector<double>& c) { data[channelId] = c; }

	void fshs(int L = 256);
	QImage toQImageGray();

	void convolution();

	std::vector<std::vector<double>> to2D(int height, int width, std::vector<double>& arr);
	std::vector<double> to1D(int height, int width, std::vector<std::vector<double>>& arr);
	std::vector<std::vector<double>> to255(int height, int width, std::vector<std::vector<double>>& arr);
};

class ImageProcessing {
private:
	
public:
	ImageProcessing() {};
	~ImageProcessing() {};

	void linDiffusionExplicite(ImageData& im, int N, vector<vector<vector<double>>>& history);
	void linDiffusionImplicit(ImageData& im, int N, vector<vector<vector<double>>>& history);
	void nonLinPeronaMalikSemiImplicit(ImageData& im, int N, vector<vector<vector<double>>>& history);
};