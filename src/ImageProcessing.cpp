#include "ImageProcessing.h"
#include <QImage.h>

using namespace std;

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

void ImageData::findMinMax(vector<double>& arr, double& min, double& max)
{
	int size = arr.size();

	for (int i = 0; i < size; i++) {
		if (arr[i] < min)
			min = arr[i];
		if (arr[i] > max)
			max = arr[i];
	}
}


void ImageData::fshs(int L)
{
	double max = -1;
	double min = 2;

	findMinMax(data[0], min, max);

	if (min == max) {
		cout << "min == max" << endl;
		return;
	}

	std::cout << "min=" << min << " max=" << max << std::endl;

	std::vector<double> dataN = data[0];
	int size = (int)dataN.size();

	for (int i = 0; i < size; i++) {
		dataN[i] = ((data[0][i] - min) * 255.0) / (max - min);
	}

	data[0] = dataN;
}

QImage ImageData::toQImageGray()
{
	const int w = getWidth();
	const int h = getHeight();

	const std::vector<double>& ch0 = getData(0);

	QImage out(w, h, QImage::Format_Grayscale8);
	if (out.isNull()) return QImage();

	uchar* dst = out.bits();
	const int bpl = out.bytesPerLine();

	for (int y = 0; y < h; ++y) {
		uchar* row = dst + y * bpl;
		const long base = (long)y * w;
		for (int x = 0; x < w; ++x) {
			int pix = (int)std::lround(ch0[base + x]);
			pix = std::clamp(pix, 0, 255);
			row[x] = (uchar)pix;
		}
	}
	return out;
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