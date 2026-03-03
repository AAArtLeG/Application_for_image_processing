#include "ImageProcessing.h"

ImageData::ImageData(int w, int h, const unsigned char* d, int bytesPerLine, int colorDepth)
{
	width = w;
	height = h;

	size = width * height;
	int channelsSize = colorDepth / 8;
	data.resize(channelsSize);

	for (int j = 0; j < channelsSize; j++)
	{
		std::vector<double>& channelData = data[j];
		channelData.resize(size);

		for (int y = 0; y < height; y++)
		{
			const unsigned char* row = d + y * bytesPerLine;

			for (int x = 0; x < width; x++)
			{
				const unsigned char* px = row + x * channelsSize;

				long index = y * width + x;
				channelData[index] = px[j] / 255.0;
			}
		}
	}
}

bool saveToPgm(const std::string& filename, int width, int height, const std::vector<double>& data)
{
	std::ofstream f(filename);
	if (!f.is_open()) {
		return false;
	}

	f << "P2\n";
	f << width << " " << height << "\n";
	f << 255 << "\n";

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			long index = y * width + x;
			f << static_cast<int>(data[index] * 255 + 0.5) << (x + 1 == width ? '\n' : ' ');
		}
	}
	f.close();
	return true;
}