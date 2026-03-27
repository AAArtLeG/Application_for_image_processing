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

vector<vector<double>> ImageData::to2D(int height, int width, vector<double>& arr) {
	vector<vector<double>> mat(height, vector<double>(width));

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			const int index = y * width + x;
			mat[y][x] = arr[index];
		}
	}

	return mat;
}

vector<double> ImageData::to1D(int height, int width, vector<vector<double>>& arr) {

	vector<double> mat(height * width);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			const int index = y * width + x;
			mat[index] = arr[y][x];
		}
	}

	return mat;
}

vector<vector<double>> ImageData::to255(int height, int width, vector<vector<double>>& arr) {
	vector<vector<double>> mat(height, vector<double>(width));

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			mat[i][j] = static_cast<int>(arr[i][j] * 255.0);
		}
	}

	return mat;
}

vector<vector<double>> ImageData::mirroring(vector<double>& arr) {
	vector<double>& ch0 = data[0];

	vector<vector<double>> mat = to2D(height, width, ch0);

	int hN = height + 4;
	int wN = width + 4;

	//cout << height << " " << width << endl;

	vector<vector<double>> matResized(hN, vector<double>(wN));

	for (int i = 0; i < hN; i++) {
		for (int j = 0; j < wN; j++) {
			matResized[i][j] = -1;
		}
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (i == 0) {
				matResized[i][j + 2] = mat[i + 1][j];
				matResized[i + 1][j + 2] = mat[i][j];
			}
			if (j == 0) {
				matResized[i + 2][j] = mat[i][j + 1];
				matResized[i + 2][j + 1] = mat[i][j];
			}
			if (i == (height - 1)) {
				matResized[i + 3][j + 2] = mat[i][j];
				matResized[i + 4][j + 2] = mat[i - 1][j];

			}
			if (j == (width - 1)) {
				matResized[i + 2][j + 4] = mat[i][j];
				matResized[i + 2][j + 3] = mat[i][j-1];
			}


			matResized[i + 2][j + 2] = mat[i][j];
				
		}
	}

	matResized[0][0] = matResized[1][2];
	matResized[0][1] = matResized[0][2];
	matResized[1][0] = matResized[2][0];
	matResized[1][1] = matResized[1][2];

	matResized[0][wN - 2] = matResized[0][wN - 3];
	matResized[0][wN - 1] = matResized[1][wN - 3];
	matResized[1][wN - 2] = matResized[1][wN - 3];
	matResized[1][wN - 1] = matResized[2][wN - 1];

	matResized[hN - 2][0] = matResized[hN - 3][0];
	matResized[hN - 2][1] = matResized[hN - 3][1];
	matResized[hN - 1][0] = matResized[hN - 3][1];
	matResized[hN - 1][1] = matResized[hN - 1][2];

	matResized[hN - 2][wN - 2] = matResized[hN - 3][wN - 2];
	matResized[hN - 2][wN - 1] = matResized[hN- 3][wN - 1];
	matResized[hN - 1][wN - 2] = matResized[hN - 1][wN - 3];
	matResized[hN - 1][wN - 1] = matResized[hN - 3][wN - 2];

	
	for (int i = 0; i < 32; i++) {
		cout << -1 << " " << -1 << " ";
		for (int j = 0; j < 32; j++) {
			cout << mat[i][j] << " ";
		}
		cout << endl;
	}

	cout << endl;
	cout << endl;

	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			cout << matResized[i][j] << " ";

		}
		cout << endl;
	}

	/*int counter = 0;
	for (int i = 0; i < 1; i++) {
		for (int j = 0; j < height; j++) {
			if (i == 0) {
				if ()
			}
		}
		cout << endl;
	}*/

	/*for (int i = 0; i < height; i++) {
	  for (int j = 0; j < width; j++) {

	  }
	}*/

	return matResized;
}

void ImageData::convolution() {
	vector<vector<double>> resized = mirroring(data[0]);

	int hN = height + 4;
	int wN = width + 4;

	vector<vector<double>> final(height, vector<double>(width));


	double sum = 0.0;
	for (int k = 0; k < height; k++) {
		for (int l = 0; l < width; l++) {
			sum = 0.0;
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 5; j++) {
					sum += kernel[i][j]*resized[k + i][l + j];
				}
			}

			final[k][l] = sum;
		}
	}

	/*double sum = 0.0;
	for (int k = 0; k < height; k++) {
		for (int l = 0; l < width; l++) {
			sum = 0.0;
			for (int m = -2; m < 3; m++) {
				for (int n = -2; n < 3; n++) {
					int i = k - m;
					int j = l - n;
					sum += kernel[m + 2][n + 2] * resized[i + 2][j + 2];
				}
			}

			final[k][l] = sum;
		}
	}*/

	vector<vector<double>> finalPgm = to255(height, width, final);

	vector<double> finalPgm1D = to1D(height, width, finalPgm);

	data[0] = finalPgm1D;
}

void ImageProcessing::linDiffusionExplicite(ImageData& im, int N) {
	vector<vector<double>> dataOrigin = im.getData();
	vector<vector<double>> dataNew = im.getData();
	int channelSize = dataOrigin.size();
	int H = im.getHeight();
	int W = im.getWidth();

	//cout << channelData << endl;

	vector<double> channel;
	vector<vector<double>> finalChanPgm;
	vector<double> finalChanPgm1D;
	vector<vector<double>> prev;
	vector<vector<double>> next(H, vector<double>(W));

	double tau = 0.2;

	int h = 1;

	double c = tau / (h * h);
	//tempForLinDif[0] = im;
	for (int ch = 0; ch < channelSize; ch++) {
		channel = dataOrigin[ch];

		prev = im.to2D(H, W, channel);
		for (int n = 0; n < N; n++) {
			next = vector<vector<double>>(H, vector<double>(W, 0.0));
			for (int i = 0; i < H; ++i) {
				for (int j = 0; j < W; ++j) {
			
					if (j + 1 > W - 1) { //2
						if (i - 1 < 0) { //5
							next[i][j] = (1 - ((2 * tau) / (h * h))) * prev[i][j];
							next[i][j] += c * prev[i][j - 1] + c * prev[i + 1][j];
							continue;
						}
						if (i + 1 > H - 1) { //6
							next[i][j] = (1 - ((2 * tau) / (h * h))) * prev[i][j];
							next[i][j] += c * prev[i][j - 1] + c * prev[i - 1][j];
							continue;
						}
						next[i][j] = (1 - ((3 * tau) / (h * h))) * prev[i][j];
						next[i][j] += c * prev[i][j - 1] + c * prev[i + 1][j] + c * prev[i - 1][j];
						continue;
					}
					if (i - 1 < 0) { // 1
						if (j + 1 > W - 1) { // 5
							next[i][j] = (1 - ((2 * tau) / (h * h))) * prev[i][j];
							next[i][j] += c * prev[i][j - 1] + c * prev[i + 1][j];
							continue;
						}
						if (j - 1 < 0) { //8
							next[i][j] = (1 - ((2 * tau) / (h * h))) * prev[i][j];
							next[i][j] += c * prev[i][j + 1] + c * prev[i + 1][j];
							continue;
						}
						next[i][j] = (1 - ((3 * tau) / (h * h))) * prev[i][j];
						next[i][j] += c * prev[i][j - 1] + c * prev[i + 1][j] + c * prev[i][j + 1];
						continue;
					}
					if (j - 1 < 0) { //4
						if (i - 1 < 0) { //8
							next[i][j] = (1 - ((2 * tau) / (h * h))) * prev[i][j];
							next[i][j] += c * prev[i][j + 1] + c * prev[i + 1][j];
							continue;
						}
						if (i + 1 > H - 1) { //7
							next[i][j] = (1 - ((2 * tau) / (h * h))) * prev[i][j];
							next[i][j] += c * prev[i][j + 1] + c * prev[i - 1][j];
							continue;
						}
						next[i][j] = (1 - ((3 * tau) / (h * h))) * prev[i][j];
						next[i][j] += c * prev[i - 1][j] + c * prev[i + 1][j] + c * prev[i][j + 1];
						continue;
					}
					if (i + 1 > H - 1) { // 3
						if (j - 1 < 0) { // 7
							next[i][j] = (1 - ((2 * tau) / (h * h))) * prev[i][j];
							next[i][j] += c * prev[i][j + 1] + c * prev[i - 1][j];
							continue;
						}
						if (j + 1 > W - 1) { //6
							next[i][j] = (1 - ((2 * tau) / (h * h))) * prev[i][j];
							next[i][j] += c * prev[i][j - 1] + c * prev[i - 1][j];
							continue;
						}
						next[i][j] = (1 - ((3 * tau) / (h * h))) * prev[i][j];
						next[i][j] += c * prev[i][j + 1] + c * prev[i - 1][j] + c * prev[i][j - 1];
						continue;
					}
					//A.insert(idx, idx + 1) = -1.0;//i,j+1
					//A.insert(idx, idx - 1) = -1.0;//i,j-1
					//A.insert(idx, idx + cols) = -1.0;//i+1,j
					//A.insert(idx, idx - cols) = -1.0;//i-1,j
					////b(idx) = matrixInPrevTime[i][j];
					//b(idx) = 0;
					next[i][j] = (1 - ((4 * tau) / (h * h))) * prev[i][j] + c * prev[i][j - 1] + c * prev[i + 1][j] + c*prev[i][j + 1] + c * prev[i - 1][j];
				}
			}

			prev = next;
		}
		finalChanPgm = im.to255(H, W, next);
		finalChanPgm1D = im.to1D(H, W, finalChanPgm);
		dataOrigin[ch] = finalChanPgm1D;
	}

	im.setData(dataOrigin);
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