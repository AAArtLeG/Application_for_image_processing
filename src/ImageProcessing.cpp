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

void ImageProcessing::linDiffusionExplicite(ImageData& im, int N, vector<vector<vector<double>>>& history, double Tau) {
	vector<vector<double>> dataOrigin = im.getData();
	vector<vector<double>> dataNew = im.getData();
	int channelSize = dataOrigin.size();
	int H = im.getHeight();
	int W = im.getWidth();

	//cout << channelData << endl;
	cout << "explicit" << endl;
	
	vector<double> channel;
	vector<vector<double>> finalChanPgm;
	vector<double> finalChanPgm1D;
	vector<vector<double>> prev;
	vector<vector<double>> next(H, vector<double>(W));
	history.assign(N, vector<vector<double>>(channelSize, vector<double>(H * W, 0.0)));
	double tau = Tau;

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
			finalChanPgm = im.to255(H, W, next);
			finalChanPgm1D = im.to1D(H, W, finalChanPgm);
			history[n][ch] = finalChanPgm1D;
		}
		finalChanPgm = im.to255(H, W, next);
		finalChanPgm1D = im.to1D(H, W, finalChanPgm);
		dataOrigin[ch] = finalChanPgm1D;
	}

	im.setData(dataOrigin);
}

void ImageProcessing::linDiffusionImplicit(ImageData& im, int N, vector<vector<vector<double>>>& history, double Tau) {
	vector<vector<double>> dataOrigin = im.getData();
	int channelSize = dataOrigin.size();
	int H = im.getHeight();
	int W = im.getWidth();

	vector<double> channel;
	vector<vector<double>> finalChanPgm;
	vector<double> finalChanPgm1D;

	vector<vector<double>> prev;
	vector<vector<double>> curr;
	vector<vector<double>> next(H, vector<double>(W));

	history.assign(N, vector<vector<double>>(channelSize, vector<double>(H * W, 0.0)));

	double tol = 1e-4;
	int maxIter = 100;

	double tau = Tau;
	int h = 1;
	double c = tau / (h * h);
	
	double omega = 1.3;

	for (int ch = 0; ch < channelSize; ch++) {
        channel = dataOrigin[ch];
		prev = im.to2D(H, W, channel);
		curr = prev;

		for (int iter = 0; iter < maxIter; iter++) {
			double residualSq = 0.0;

			cout << iter << endl;

			for (int i = 0; i < H; ++i) {
				for (int j = 0; j < W; ++j) {

					double oldValue = curr[i][j];

					if (j + 1 > W - 1) { // 2
						if (i - 1 < 0) { // 5
							double diag = 1.0 + 2.0 * c;
							double sum = (prev[i][j]
								+ c * curr[i][j - 1]
								+ c * curr[i + 1][j]) / diag;

							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- c * curr[i][j - 1]
								- c * curr[i + 1][j]
								- prev[i][j];
							residualSq += r_i * r_i;
							continue;
						}

						if (i + 1 > H - 1) { // 6
							double diag = 1.0 + 2.0 * c;
							double sum = (prev[i][j]
								+ c * curr[i][j - 1]
								+ c * curr[i - 1][j]) / diag;

							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- c * curr[i][j - 1]
								- c * curr[i - 1][j]
								- prev[i][j];
							residualSq += r_i * r_i;
							continue;
						}

						double diag = 1.0 + 3.0 * c;
						double sum = (prev[i][j]
							+ c * curr[i][j - 1]
							+ c * curr[i + 1][j]
							+ c * curr[i - 1][j]) / diag;

						double newValue = oldValue + omega * (sum - oldValue);
						curr[i][j] = newValue;

						double r_i = diag * oldValue
							- c * curr[i][j - 1]
							- c * curr[i + 1][j]
							- c * curr[i - 1][j]
							- prev[i][j];
						residualSq += r_i * r_i;
						continue;
					}

					if (i - 1 < 0) { // 1
						if (j + 1 > W - 1) { // 5
							double diag = 1.0 + 2.0 * c;
							double sum = (prev[i][j]
								+ c * curr[i][j - 1]
								+ c * curr[i + 1][j]) / diag;

							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- c * curr[i][j - 1]
								- c * curr[i + 1][j]
								- prev[i][j];
							residualSq += r_i * r_i;
							continue;
						}

						if (j - 1 < 0) { // 8
							double diag = 1.0 + 2.0 * c;
							double sum = (prev[i][j]
								+ c * curr[i][j + 1]
								+ c * curr[i + 1][j]) / diag;

							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- c * curr[i][j + 1]
								- c * curr[i + 1][j]
								- prev[i][j];
							residualSq += r_i * r_i;
							continue;
						}

						double diag = 1.0 + 3.0 * c;
						double sum = (prev[i][j]
							+ c * curr[i][j - 1]
							+ c * curr[i + 1][j]
							+ c * curr[i][j + 1]) / diag;

						double newValue = oldValue + omega * (sum - oldValue);
						curr[i][j] = newValue;

						double r_i = diag * oldValue
							- c * curr[i][j - 1]
							- c * curr[i + 1][j]
							- c * curr[i][j + 1]
							- prev[i][j];
						residualSq += r_i * r_i;
						continue;
					}

					if (j - 1 < 0) { // 4
						if (i - 1 < 0) { // 8
							double diag = 1.0 + 2.0 * c;
							double sum = (prev[i][j]
								+ c * curr[i][j + 1]
								+ c * curr[i + 1][j]) / diag;

							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- c * curr[i][j + 1]
								- c * curr[i + 1][j]
								- prev[i][j];
							residualSq += r_i * r_i;
							continue;
						}

						if (i + 1 > H - 1) { // 7
							double diag = 1.0 + 2.0 * c;
							double sum = (prev[i][j]
								+ c * curr[i][j + 1]
								+ c * curr[i - 1][j]) / diag;

							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- c * curr[i][j + 1]
								- c * curr[i - 1][j]
								- prev[i][j];
							residualSq += r_i * r_i;
							continue;
						}

						double diag = 1.0 + 3.0 * c;
						double sum = (prev[i][j]
							+ c * curr[i - 1][j]
							+ c * curr[i + 1][j]
							+ c * curr[i][j + 1]) / diag;

						double newValue = oldValue + omega * (sum - oldValue);
						curr[i][j] = newValue;

						double r_i = diag * oldValue
							- c * curr[i - 1][j]
							- c * curr[i + 1][j]
							- c * curr[i][j + 1]
							- prev[i][j];
						residualSq += r_i * r_i;
						continue;
					}

					if (i + 1 > H - 1) { // 3
						if (j - 1 < 0) { // 7
							double diag = 1.0 + 2.0 * c;
							double sum = (prev[i][j]
								+ c * curr[i][j + 1]
								+ c * curr[i - 1][j]) / diag;

							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- c * curr[i][j + 1]
								- c * curr[i - 1][j]
								- prev[i][j];
							residualSq += r_i * r_i;
							continue;
						}

						if (j + 1 > W - 1) { // 6
							double diag = 1.0 + 2.0 * c;
							double sum = (prev[i][j]
								+ c * curr[i][j - 1]
								+ c * curr[i - 1][j]) / diag;

							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- c * curr[i][j - 1]
								- c * curr[i - 1][j]
								- prev[i][j];
							residualSq += r_i * r_i;
							continue;
						}

						double diag = 1.0 + 3.0 * c;
						double sum = (prev[i][j]
							+ c * curr[i][j + 1]
							+ c * curr[i - 1][j]
							+ c * curr[i][j - 1]) / diag;

						double newValue = oldValue + omega * (sum - oldValue);
						curr[i][j] = newValue;

						double r_i = diag * oldValue
							- c * curr[i][j + 1]
							- c * curr[i - 1][j]
							- c * curr[i][j - 1]
							- prev[i][j];
						residualSq += r_i * r_i;
						continue;
					}

					// vnútorný bod
					double diag = 1.0 + 4.0 * c;
					double sum = (prev[i][j]
						+ c * curr[i][j - 1]
						+ c * curr[i + 1][j]
						+ c * curr[i][j + 1]
						+ c * curr[i - 1][j]) / diag;

					double newValue = oldValue + omega * (sum - oldValue);
					curr[i][j] = newValue;

					double r_i = diag * oldValue
						- c * curr[i][j - 1]
						- c * curr[i + 1][j]
						- c * curr[i][j + 1]
						- c * curr[i - 1][j]
						- prev[i][j];
					residualSq += r_i * r_i;
				}
			}

			next = curr;
			prev = next;

			finalChanPgm = im.to255(H, W, next);
			finalChanPgm1D = im.to1D(H, W, finalChanPgm);
			//history[iter][ch] = finalChanPgm1D;

			double residual = sqrt(residualSq);
			if (residual < tol) {
				break;
			}

		}

        finalChanPgm = im.to255(H, W, next);
        finalChanPgm1D = im.to1D(H, W, finalChanPgm);
        dataOrigin[ch] = finalChanPgm1D;
		history[0][ch] = finalChanPgm1D;
    }

	im.setData(dataOrigin);
}

void ImageProcessing::nonLinPeronaMalikSemiImplicit(ImageData& im, int N, vector<vector<vector<double>>>& history, double Tau) {
	vector<vector<double>> dataOrigin = im.getData();
	int channelSize = dataOrigin.size();
	int H = im.getHeight();
	int W = im.getWidth();

	vector<double> channel;
	vector<vector<double>> finalChanPgm;
	vector<double> finalChanPgm1D;

	double tau = Tau;     
	double sigma = 0.25;    
	double K = 200.0; 
	int h = 1;

	history.assign(N, vector<vector<double>>(channelSize, vector<double>(H * W, 0.0)));

	double tol = 1e-4;
	int    maxIter = 100;
	double omega = 1.3;

	double c = tau / (h * h);
	double cSigma = sigma / (h * h);
	
	for (int ch = 0; ch < channelSize; ch++) {
		channel = dataOrigin[ch];

		vector<vector<double>> prev = im.to2D(H, W, channel);

		for (int n = 0; n < N; n++) {
			vector<vector<double>> uSigma(H, vector<double>(W, 0.0));


			for (int i = 0; i < H; ++i) {
				for (int j = 0; j < W; ++j) {

					if (j + 1 > W - 1) { //2
						if (i - 1 < 0) { //5
							uSigma[i][j] = (1 - ((2 * tau) / (h * h))) * prev[i][j];
							uSigma[i][j] += cSigma * prev[i][j - 1] + cSigma * prev[i + 1][j];
							continue;
						}
						if (i + 1 > H - 1) { //6
							uSigma[i][j] = (1 - ((2 * tau) / (h * h))) * prev[i][j];
							uSigma[i][j] += cSigma * prev[i][j - 1] + cSigma * prev[i - 1][j];
							continue;
						}
						uSigma[i][j] = (1 - ((3 * tau) / (h * h))) * prev[i][j];
						uSigma[i][j] += cSigma * prev[i][j - 1] + cSigma * prev[i + 1][j] + cSigma * prev[i - 1][j];
						continue;
					}
					if (i - 1 < 0) { // 1
						if (j + 1 > W - 1) { // 5
							uSigma[i][j] = (1 - ((2 * tau) / (h * h))) * prev[i][j];
							uSigma[i][j] += cSigma * prev[i][j - 1] + cSigma * prev[i + 1][j];
							continue;
						}
						if (j - 1 < 0) { //8
							uSigma[i][j] = (1 - ((2 * tau) / (h * h))) * prev[i][j];
							uSigma[i][j] += cSigma * prev[i][j + 1] + cSigma * prev[i + 1][j];
							continue;
						}
						uSigma[i][j] = (1 - ((3 * tau) / (h * h))) * prev[i][j];
						uSigma[i][j] += cSigma * prev[i][j - 1] + cSigma * prev[i + 1][j] + cSigma * prev[i][j + 1];
						continue;
					}
					if (j - 1 < 0) { //4
						if (i - 1 < 0) { //8
							uSigma[i][j] = (1 - ((2 * tau) / (h * h))) * prev[i][j];
							uSigma[i][j] += cSigma * prev[i][j + 1] + cSigma * prev[i + 1][j];
							continue;
						}
						if (i + 1 > H - 1) { //7
							uSigma[i][j] = (1 - ((2 * tau) / (h * h))) * prev[i][j];
							uSigma[i][j] += cSigma * prev[i][j + 1] + cSigma * prev[i - 1][j];
							continue;
						}
						uSigma[i][j] = (1 - ((3 * tau) / (h * h))) * prev[i][j];
						uSigma[i][j] += cSigma * prev[i - 1][j] + cSigma * prev[i + 1][j] + cSigma * prev[i][j + 1];
						continue;
					}
					if (i + 1 > H - 1) { // 3
						if (j - 1 < 0) { // 7
							uSigma[i][j] = (1 - ((2 * tau) / (h * h))) * prev[i][j];
							uSigma[i][j] += cSigma * prev[i][j + 1] + cSigma * prev[i - 1][j];
							continue;
						}
						if (j + 1 > W - 1) { //6
							uSigma[i][j] = (1 - ((2 * tau) / (h * h))) * prev[i][j];
							uSigma[i][j] += cSigma * prev[i][j - 1] + cSigma * prev[i - 1][j];
							continue;
						}
						uSigma[i][j] = (1 - ((3 * tau) / (h * h))) * prev[i][j];
						uSigma[i][j] += cSigma * prev[i][j + 1] + cSigma * prev[i - 1][j] + cSigma * prev[i][j - 1];
						continue;
					}
					uSigma[i][j] = (1 - ((4 * tau) / (h * h))) * prev[i][j] + cSigma * prev[i][j - 1] + cSigma * prev[i + 1][j] + cSigma * prev[i][j + 1] + cSigma * prev[i - 1][j];
				}
			}

			int Hx = H + 2;
			int Wx = W + 2;
			vector<vector<double>> uSigmaBig(Hx, vector<double>(Wx, 0.0));

			for (int i = 0; i < H; ++i) { //inner
				for (int j = 0; j < W; ++j) {
					uSigmaBig[i + 1][j + 1] = uSigma[i][j];
				}
			}
			for (int j = 0; j < W; ++j) { //bot and top
				uSigmaBig[0][j + 1] = uSigma[0][j];
				uSigmaBig[Hx - 1][j + 1] = uSigma[H - 1][j];
			}
			for (int i = 0; i < H; ++i) { // left and right
				uSigmaBig[i + 1][0] = uSigma[i][0];
				uSigmaBig[i + 1][Wx - 1] = uSigma[i][W - 1];
			}

			uSigmaBig[0][0] = uSigma[0][0];
			uSigmaBig[0][Wx - 1] = uSigma[0][W - 1];
			uSigmaBig[Hx - 1][0] = uSigma[H - 1][0];
			uSigmaBig[Hx - 1][Wx - 1] = uSigma[H - 1][W - 1];

			vector<vector<double>> gPE(H, vector<double>(W, 0.0));
			vector<vector<double>> gPW(H, vector<double>(W, 0.0));
			vector<vector<double>> gPN(H, vector<double>(W, 0.0));
			vector<vector<double>> gPS(H, vector<double>(W, 0.0));

			for (int i = 0; i < H; ++i) {
				for (int j = 0; j < W; ++j) {

					int ii = i + 1;
					int jj = j + 1;

					double up = uSigmaBig[ii][jj];
					double uN = uSigmaBig[ii - 1][jj];
					double uS = uSigmaBig[ii + 1][jj];
					double uW = uSigmaBig[ii][jj - 1];
					double uE = uSigmaBig[ii][jj + 1];
					double uNE = uSigmaBig[ii - 1][jj + 1];
					double uNW = uSigmaBig[ii - 1][jj - 1];
					double uSE = uSigmaBig[ii + 1][jj + 1];
					double uSW = uSigmaBig[ii + 1][jj - 1];

					double gradPE = (((uE - up) / h) * ((uE - up) / h)) + (((uN + uNE - uS - uSE) / (4 * h)) * ((uN + uNE - uS - uSE) / (4 * h)));
					double gradPS = (((uS - up) / h) * ((uS - up) / h)) + (((uW + uSW - uE - uSE) / (4 * h)) * ((uW + uSW - uE - uSE) / (4 * h)));
					double gradPW = (((uW - up) / h) * ((uW - up) / h)) + (((uN + uNW - uS - uSW) / (4 * h)) * ((uN + uNW - uS - uSW) / (4 * h)));
					double gradPN = (((uN - up) / h) * ((uN - up) / h)) + (((uW + uNW - uE - uNE) / (4 * h)) * ((uW + uNW - uE - uNE) / (4 * h)));

					gPE[i][j] = 1.0 / (1.0 + K * gradPE);
					gPS[i][j] = 1.0 / (1.0 + K * gradPS);
					gPW[i][j] = 1.0 / (1.0 + K * gradPW);
					gPN[i][j] = 1.0 / (1.0 + K * gradPN);
					
				}
			}

			vector<vector<double>> curr = prev;   // начальное приближение
			vector<vector<double>> next(H, vector<double>(W, 0.0));

			for (int iter = 0; iter < maxIter; iter++) {
				double maxDiff = 0.0;
				double residualSq = 0.0;

				for (int i = 0; i < H; ++i) {
					for (int j = 0; j < W; ++j) {

						double oldValue = curr[i][j];

						double gn = gPN[i][j];
						double gs = gPS[i][j];
						double gw = gPW[i][j];
						double ge = gPE[i][j];

						if (j + 1 > W - 1) { // 2
							if (i - 1 < 0) { // 5
								double diag = 1.0 + c * (gs + gw);
								double sum = (prev[i][j]
									+ c * gw *curr[i][j - 1]
									+ c * gs * curr[i + 1][j]) / diag;

								double newValue = oldValue + omega * (sum - oldValue);
								curr[i][j] = newValue;

								double r_i = diag * oldValue
									- c * gw * curr[i][j - 1]
									- c * gs * curr[i + 1][j]
									- prev[i][j];
								residualSq += r_i * r_i;
								continue;
							}

							if (i + 1 > H - 1) { // 6
								double diag = 1.0 + c * (gw + gn);
								double sum = (prev[i][j]
									+ c * gw * curr[i][j - 1]
									+ c * gn * curr[i - 1][j]) / diag;

								double newValue = oldValue + omega * (sum - oldValue);
								curr[i][j] = newValue;

								double r_i = diag * oldValue
									- c * gw * curr[i][j - 1]
									- c * gn * curr[i - 1][j]
									- prev[i][j];
								residualSq += r_i * r_i;
								continue;
							}

							double diag = 1.0 + c * (gs + gw + gn);
							double sum = (prev[i][j]
								+ c * gw * curr[i][j - 1]
								+ c * gs * curr[i + 1][j]
								+ c * gn * curr[i - 1][j]) / diag;

							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- c * gw * curr[i][j - 1]
								- c * gs * curr[i + 1][j]
								- c * gn * curr[i - 1][j]
								- prev[i][j];
							residualSq += r_i * r_i;
							continue;
						}

						if (i - 1 < 0) { // 1
							if (j + 1 > W - 1) { // 5
								double diag = 1.0 + c * (gs + gw);
								double sum = (prev[i][j]
									+ c * gw * curr[i][j - 1]
									+ c * gs * curr[i + 1][j]) / diag;

								double newValue = oldValue + omega * (sum - oldValue);
								curr[i][j] = newValue;

								double r_i = diag * oldValue
									- c * gw * curr[i][j - 1]
									- c * gs * curr[i + 1][j]
									- prev[i][j];
								residualSq += r_i * r_i;
								continue;
							}

							if (j - 1 < 0) { // 8
								double diag = 1.0 + c * (gs + ge);
								double sum = (prev[i][j]
									+ c * ge * curr[i][j + 1]
									+ c * gs * curr[i + 1][j]) / diag;

								double newValue = oldValue + omega * (sum - oldValue);
								curr[i][j] = newValue;

								double r_i = diag * oldValue
									- c * ge * curr[i][j + 1]
									- c * gs * curr[i + 1][j]
									- prev[i][j];
								residualSq += r_i * r_i;
								continue;
							}

							double diag = 1.0 + c * (gs + gw + ge);
							double sum = (prev[i][j]
								+ c * gw * curr[i][j - 1]
								+ c * gs * curr[i + 1][j]
								+ c * ge * curr[i][j + 1]) / diag;
							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- c * gw * curr[i][j - 1]
								- c * gs * curr[i + 1][j]
								- c * ge * curr[i][j + 1]
								- prev[i][j];
							residualSq += r_i * r_i;
							continue;
						}

						if (j - 1 < 0) { // 4
							if (i - 1 < 0) { // 8
								double diag = 1.0 + c * (gs + ge);
								double sum = (prev[i][j]
									+ c * ge * curr[i][j + 1]
									+ c * gs * curr[i + 1][j]) / diag;

								double newValue = oldValue + omega * (sum - oldValue);
								curr[i][j] = newValue;

								double r_i = diag * oldValue
									- c * ge * curr[i][j + 1]
									- c * gs * curr[i + 1][j]
									- prev[i][j];
								residualSq += r_i * r_i;
								continue;
							}

							if (i + 1 > H - 1) { // 7
								double diag = 1.0 + c * (ge + gn);
								double sum = (prev[i][j]
									+ c * ge * curr[i][j + 1]
									+ c * gn * curr[i - 1][j]) / diag;

								double newValue = oldValue + omega * (sum - oldValue);
								curr[i][j] = newValue;

								double r_i = diag * oldValue
									- c * ge * curr[i][j + 1]
									- c * gn * curr[i - 1][j]
									- prev[i][j];
								residualSq += r_i * r_i;
								continue;
							}

							double diag = 1.0 + c * (gn + gs + ge);
							double sum = (prev[i][j]
								+ c * gn * curr[i - 1][j]
								+ c * gs * curr[i + 1][j]
								+ c * ge * curr[i][j + 1]) / diag;

							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- c * gn * curr[i - 1][j]
								- c * gs * curr[i + 1][j]
								- c * ge * curr[i][j + 1]
								- prev[i][j];
							residualSq += r_i * r_i;
							continue;
						}

						if (i + 1 > H - 1) { // 3
							if (j - 1 < 0) { // 7
								double diag = 1.0 + c * (ge + gn);
								double sum = (prev[i][j]
									+ c * ge * curr[i][j + 1]
									+ c * gn * curr[i - 1][j]) / diag;

								double newValue = oldValue + omega * (sum - oldValue);
								curr[i][j] = newValue;

								double r_i = diag * oldValue
									- c * ge * curr[i][j + 1]
									- c * gn * curr[i - 1][j]
									- prev[i][j];
								residualSq += r_i * r_i;
								continue;
							}

							if (j + 1 > W - 1) { // 6
								double diag = 1.0 + c * (gw + gn);
								double sum = (prev[i][j]
									+ c * gw * curr[i][j - 1]
									+ c * gn * curr[i - 1][j]) / diag;

								double newValue = oldValue + omega * (sum - oldValue);
								curr[i][j] = newValue;

								double r_i = diag * oldValue
									- c * gw * curr[i][j - 1]
									- c * gn * curr[i - 1][j]
									- prev[i][j];
								residualSq += r_i * r_i;
								continue;
							}

							double diag = 1.0 + c * (ge + gn + gw);
							double sum = (prev[i][j]
								+ c * ge * curr[i][j + 1]
								+ c * gn * curr[i - 1][j]
								+ c * gw * curr[i][j - 1]) / diag;
							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- c * ge * curr[i][j + 1]
								- c * gn * curr[i - 1][j]
								- c * gw * curr[i][j - 1]
								- prev[i][j];
							residualSq += r_i * r_i;
							continue;
						}

						// vnútorný bod
						double diag = 1.0 + c * (gw + gs + ge + gn);
						double sum = (prev[i][j]
							+ c * gw * curr[i][j - 1]
							+ c * gs * curr[i + 1][j]
							+ c * ge * curr[i][j + 1]
							+ c * gn * curr[i - 1][j]) / diag;

						double newValue = oldValue + omega * (sum - oldValue);
						curr[i][j] = newValue;

						double r_i = diag * oldValue
							- c * gw * curr[i][j - 1]
							- c * gs * curr[i + 1][j]
							- c * ge * curr[i][j + 1]
							- c * gn * curr[i - 1][j]
							- prev[i][j];
						residualSq += r_i * r_i;
					}
				}

				double residual = sqrt(residualSq);
				if (residual < tol) break;
			}

			next = curr;

			// u^n становится u^{n-1} для следующей итерации
			prev = next;

			// Сохраняем шаг в history
			finalChanPgm = im.to255(H, W, next);
			finalChanPgm1D = im.to1D(H, W, finalChanPgm);
			history[n][ch] = finalChanPgm1D;
		}

		finalChanPgm = im.to255(H, W, prev);
		finalChanPgm1D = im.to1D(H, W, finalChanPgm);
		dataOrigin[ch] = finalChanPgm1D;
	}

	im.setData(dataOrigin);
}

void ImageProcessing::GMCF(ImageData& im, int N, vector<vector<vector<double>>>& history, double Tau) {
	vector<vector<double>> dataOrigin = im.getData();
	int channelSize = dataOrigin.size();
	int H = im.getHeight();
	int W = im.getWidth();

	vector<double> channel;
	vector<vector<double>> finalChanPgm;
	vector<double> finalChanPgm1D;

	double tau = Tau;
	double sigma = 0.25;
	double K = 500.0;
	int h = 1;

	history.assign(N, vector<vector<double>>(channelSize, vector<double>(H * W, 0.0)));

	double tol = 1e-6;
	int    maxIter = 10000;
	double omega = 1.9;

	double c = tau / (h * h);
	double cSigma = sigma / (h * h);

	for (int ch = 0; ch < channelSize; ch++) {
		channel = dataOrigin[ch];

		vector<vector<double>> prev = im.to2D(H, W, channel);

		for (int n = 0; n < N; n++) {
			vector<vector<double>> uSigma(H, vector<double>(W, 0.0));


			for (int i = 0; i < H; ++i) {
				for (int j = 0; j < W; ++j) {

					if (j + 1 > W - 1) { //2
						if (i - 1 < 0) { //5
							uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * prev[i][j];
							uSigma[i][j] += cSigma * prev[i][j - 1] + cSigma * prev[i + 1][j];
							continue;
						}
						if (i + 1 > H - 1) { //6
							uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * prev[i][j];
							uSigma[i][j] += cSigma * prev[i][j - 1] + cSigma * prev[i - 1][j];
							continue;
						}
						uSigma[i][j] = (1 - ((3 * sigma) / (h * h))) * prev[i][j];
						uSigma[i][j] += cSigma * prev[i][j - 1] + cSigma * prev[i + 1][j] + cSigma * prev[i - 1][j];
						continue;
					}
					if (i - 1 < 0) { // 1
						if (j + 1 > W - 1) { // 5
							uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * prev[i][j];
							uSigma[i][j] += cSigma * prev[i][j - 1] + cSigma * prev[i + 1][j];
							continue;
						}
						if (j - 1 < 0) { //8
							uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * prev[i][j];
							uSigma[i][j] += cSigma * prev[i][j + 1] + cSigma * prev[i + 1][j];
							continue;
						}
						uSigma[i][j] = (1 - ((3 * sigma) / (h * h))) * prev[i][j];
						uSigma[i][j] += cSigma * prev[i][j - 1] + cSigma * prev[i + 1][j] + cSigma * prev[i][j + 1];
						continue;
					}
					if (j - 1 < 0) { //4
						if (i - 1 < 0) { //8
							uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * prev[i][j];
							uSigma[i][j] += cSigma * prev[i][j + 1] + cSigma * prev[i + 1][j];
							continue;
						}
						if (i + 1 > H - 1) { //7
							uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * prev[i][j];
							uSigma[i][j] += cSigma * prev[i][j + 1] + cSigma * prev[i - 1][j];
							continue;
						}
						uSigma[i][j] = (1 - ((3 * sigma) / (h * h))) * prev[i][j];
						uSigma[i][j] += cSigma * prev[i - 1][j] + cSigma * prev[i + 1][j] + cSigma * prev[i][j + 1];
						continue;
					}
					if (i + 1 > H - 1) { // 3
						if (j - 1 < 0) { // 7
							uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * prev[i][j];
							uSigma[i][j] += cSigma * prev[i][j + 1] + cSigma * prev[i - 1][j];
							continue;
						}
						if (j + 1 > W - 1) { //6
							uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * prev[i][j];
							uSigma[i][j] += cSigma * prev[i][j - 1] + cSigma * prev[i - 1][j];
							continue;
						}
						uSigma[i][j] = (1 - ((3 * sigma) / (h * h))) * prev[i][j];
						uSigma[i][j] += cSigma * prev[i][j + 1] + cSigma * prev[i - 1][j] + cSigma * prev[i][j - 1];
						continue;
					}
					uSigma[i][j] = (1 - ((4 * sigma) / (h * h))) * prev[i][j] + cSigma * prev[i][j - 1] + cSigma * prev[i + 1][j] + cSigma * prev[i][j + 1] + cSigma * prev[i - 1][j];
				}
			}

			int Hx = H + 2;
			int Wx = W + 2;
			vector<vector<double>> uSigmaBig(Hx, vector<double>(Wx, 0.0));

			for (int i = 0; i < H; ++i) { //inner
				for (int j = 0; j < W; ++j) {
					uSigmaBig[i + 1][j + 1] = uSigma[i][j];
				}
			}
			for (int j = 0; j < W; ++j) { //bot and top
				uSigmaBig[0][j + 1] = uSigma[0][j];
				uSigmaBig[Hx - 1][j + 1] = uSigma[H - 1][j];
			}
			for (int i = 0; i < H; ++i) { // left and right
				uSigmaBig[i + 1][0] = uSigma[i][0];
				uSigmaBig[i + 1][Wx - 1] = uSigma[i][W - 1];
			}

			uSigmaBig[0][0] = uSigma[0][0];
			uSigmaBig[0][Wx - 1] = uSigma[0][W - 1];
			uSigmaBig[Hx - 1][0] = uSigma[H - 1][0];
			uSigmaBig[Hx - 1][Wx - 1] = uSigma[H - 1][W - 1];

			vector<vector<double>> gPE(H, vector<double>(W, 0.0));
			vector<vector<double>> gPW(H, vector<double>(W, 0.0));
			vector<vector<double>> gPN(H, vector<double>(W, 0.0));
			vector<vector<double>> gPS(H, vector<double>(W, 0.0));

			for (int i = 0; i < H; ++i) {
				for (int j = 0; j < W; ++j) {

					int ii = i + 1;
					int jj = j + 1;

					double up = uSigmaBig[ii][jj];
					double uN = uSigmaBig[ii - 1][jj];
					double uS = uSigmaBig[ii + 1][jj];
					double uW = uSigmaBig[ii][jj - 1];
					double uE = uSigmaBig[ii][jj + 1];
					double uNE = uSigmaBig[ii - 1][jj + 1];
					double uNW = uSigmaBig[ii - 1][jj - 1];
					double uSE = uSigmaBig[ii + 1][jj + 1];
					double uSW = uSigmaBig[ii + 1][jj - 1];

					double gradPE = (((uE - up) / h) * ((uE - up) / h)) + (((uN + uNE - uS - uSE) / (4 * h)) * ((uN + uNE - uS - uSE) / (4 * h)));
					double gradPS = (((uS - up) / h) * ((uS - up) / h)) + (((uW + uSW - uE - uSE) / (4 * h)) * ((uW + uSW - uE - uSE) / (4 * h)));
					double gradPW = (((uW - up) / h) * ((uW - up) / h)) + (((uN + uNW - uS - uSW) / (4 * h)) * ((uN + uNW - uS - uSW) / (4 * h)));
					double gradPN = (((uN - up) / h) * ((uN - up) / h)) + (((uW + uNW - uE - uNE) / (4 * h)) * ((uW + uNW - uE - uNE) / (4 * h)));

					gPE[i][j] = 1.0 / (1.0 + K * gradPE);
					gPS[i][j] = 1.0 / (1.0 + K * gradPS);
					gPW[i][j] = 1.0 / (1.0 + K * gradPW);
					gPN[i][j] = 1.0 / (1.0 + K * gradPN);

				}
			}

			vector<vector<double>> uPrevBig(Hx, vector<double>(Wx, 0.0));

			for (int i = 0; i < H; ++i) { //inner
				for (int j = 0; j < W; ++j) {
					uPrevBig[i + 1][j + 1] = prev[i][j];
				}
			}
			for (int j = 0; j < W; ++j) { //bot and top
				uPrevBig[0][j + 1] = prev[0][j];
				uPrevBig[Hx - 1][j + 1] = prev[H - 1][j];
			}
			for (int i = 0; i < H; ++i) { // left and right
				uPrevBig[i + 1][0] = prev[i][0];
				uPrevBig[i + 1][Wx - 1] = prev[i][W - 1];
			}

			uPrevBig[0][0] = prev[0][0];
			uPrevBig[0][Wx - 1] = prev[0][W - 1];
			uPrevBig[Hx - 1][0] = prev[H - 1][0];
			uPrevBig[Hx - 1][Wx - 1] = prev[H - 1][W - 1];

			vector<vector<double>> gradPEEps(H, vector<double>(W, 0.0));
			vector<vector<double>> gradPWEps(H, vector<double>(W, 0.0));
			vector<vector<double>> gradPNEps(H, vector<double>(W, 0.0));
			vector<vector<double>> gradPSEps(H, vector<double>(W, 0.0));

			double eps = 1e-3;
			for (int i = 0; i < H; ++i) {
				for (int j = 0; j < W; ++j) {

					int ii = i + 1;
					int jj = j + 1;

					double up = uPrevBig[ii][jj];
					double uN = uPrevBig[ii - 1][jj];
					double uS = uPrevBig[ii + 1][jj];
					double uW = uPrevBig[ii][jj - 1];
					double uE = uPrevBig[ii][jj + 1];
					double uNE = uPrevBig[ii - 1][jj + 1];
					double uNW = uPrevBig[ii - 1][jj - 1];
					double uSE = uPrevBig[ii + 1][jj + 1];
					double uSW = uPrevBig[ii + 1][jj - 1];

					double gradPE = (((uE - up) / h) * ((uE - up) / h)) + (((uN + uNE - uS - uSE) / (4 * h)) * ((uN + uNE - uS - uSE) / (4 * h)));
					double gradPS = (((uS - up) / h) * ((uS - up) / h)) + (((uW + uSW - uE - uSE) / (4 * h)) * ((uW + uSW - uE - uSE) / (4 * h)));
					double gradPW = (((uW - up) / h) * ((uW - up) / h)) + (((uN + uNW - uS - uSW) / (4 * h)) * ((uN + uNW - uS - uSW) / (4 * h)));
					double gradPN = (((uN - up) / h) * ((uN - up) / h)) + (((uW + uNW - uE - uNE) / (4 * h)) * ((uW + uNW - uE - uNE) / (4 * h)));

					gradPEEps[i][j] = sqrt((eps * eps) + (gradPE));
					gradPSEps[i][j] = sqrt((eps * eps) + (gradPS));
					gradPWEps[i][j] = sqrt((eps * eps) + (gradPW));
					gradPNEps[i][j] = sqrt((eps * eps) + (gradPN));

				}
			}

			vector<vector<double>> gradEpsAvg(H, vector<double>(W, 0.0));

			for (int i = 0; i < H; ++i) {
				for (int j = 0; j < W; ++j) {
					double sum = 0.0;
					double count = 0.0;

					if (i - 1 >= 0) { sum += gradPNEps[i][j]; count += 1.0; }
					if (i + 1 <= H - 1) { sum += gradPSEps[i][j]; count += 1.0; }
					if (j - 1 >= 0) { sum += gradPWEps[i][j]; count += 1.0; }
					if (j + 1 <= W - 1) { sum += gradPEEps[i][j]; count += 1.0; }

					gradEpsAvg[i][j] = sum / count;
				}
			}

			vector<vector<double>> curr = prev;   // начальное приближение
			vector<vector<double>> next(H, vector<double>(W, 0.0));

			for (int iter = 0; iter < maxIter; iter++) {
				double maxDiff = 0.0;
				double residualSq = 0.0;

				for (int i = 0; i < H; ++i) {
					for (int j = 0; j < W; ++j) {

						c = tau / (h * h);

						double oldValue = curr[i][j];

						double gn = gPN[i][j];
						double gs = gPS[i][j];
						double gw = gPW[i][j];
						double ge = gPE[i][j];

						double gradN = gradPNEps[i][j];
						double gradS = gradPSEps[i][j];
						double gradW = gradPWEps[i][j];
						double gradE = gradPEEps[i][j];

						double vagN = gn / gradN;
						double vagS = gs / gradS;
						double vagW = gw / gradW;
						double vagE = ge / gradE;

						double gradAvg = gradEpsAvg[i][j];

						c *= gradAvg;

						

						if (j + 1 > W - 1) { // 2
							if (i - 1 < 0) { // 5
								double diag = 1.0 + c * (vagS + vagW);
								double sum = (prev[i][j]
									+ c * vagW * curr[i][j - 1]
									+ c * vagS * curr[i + 1][j]) / diag;

								double newValue = oldValue + omega * (sum - oldValue);
								curr[i][j] = newValue;

								double r_i = diag * oldValue
									- c * vagW * curr[i][j - 1]
									- c * vagS * curr[i + 1][j]
									- prev[i][j];
								residualSq += r_i * r_i;
								continue;
							}

							if (i + 1 > H - 1) { // 6
								double diag = 1.0 + c * (vagW + vagN);
								double sum = (prev[i][j]
									+ c * vagW * curr[i][j - 1]
									+ c * vagN * curr[i - 1][j]) / diag;
								double newValue = oldValue + omega * (sum - oldValue);
								curr[i][j] = newValue;

								double r_i = diag * oldValue
									- c * vagW * curr[i][j - 1]
									- c * vagN * curr[i - 1][j]
									- prev[i][j];
								residualSq += r_i * r_i;
								continue;
							}

							double diag = 1.0 + c * (vagS + vagW + vagN);
							double sum = (prev[i][j]
								+ c * vagW * curr[i][j - 1]
								+ c * vagS * curr[i + 1][j]
								+ c * vagN * curr[i - 1][j]) / diag;

							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- c * vagW * curr[i][j - 1]
								- c * vagS * curr[i + 1][j]
								- c * vagN * curr[i - 1][j]
								- prev[i][j];
							residualSq += r_i * r_i;
							continue;
						}

						if (i - 1 < 0) { // 1
							if (j + 1 > W - 1) { // 5
								double diag = 1.0 + c * (vagS + vagW);
								double sum = (prev[i][j]
									+ c * vagW * curr[i][j - 1]
									+ c * vagS * curr[i + 1][j]) / diag;

								double newValue = oldValue + omega * (sum - oldValue);
								curr[i][j] = newValue;

								double r_i = diag * oldValue
									- c * vagW * curr[i][j - 1]
									- c * vagS * curr[i + 1][j]
									- prev[i][j];
								residualSq += r_i * r_i;
								continue;
							}

							if (j - 1 < 0) { // 8
								double diag = 1.0 + c * (vagS + vagE);
								double sum = (prev[i][j]
									+ c * vagE * curr[i][j + 1]
									+ c * vagS * curr[i + 1][j]) / diag;

								double newValue = oldValue + omega * (sum - oldValue);
								curr[i][j] = newValue;

								double r_i = diag * oldValue
									- c * vagE * curr[i][j + 1]
									- c * vagS * curr[i + 1][j]
									- prev[i][j];
								residualSq += r_i * r_i;
								continue;
							}

							double diag = 1.0 + c * (vagS + vagW + vagE);
							double sum = (prev[i][j]
								+ c * vagW * curr[i][j - 1]
								+ c * vagS * curr[i + 1][j]
								+ c * vagE * curr[i][j + 1]) / diag;
							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- c * vagW * curr[i][j - 1]
								- c * vagS * curr[i + 1][j]
								- c * vagE * curr[i][j + 1]
								- prev[i][j];
							residualSq += r_i * r_i;
							continue;
						}

						if (j - 1 < 0) { // 4
							if (i - 1 < 0) { // 8
								double diag = 1.0 + c * (vagS + vagE);
								double sum = (prev[i][j]
									+ c * vagE * curr[i][j + 1]
									+ c * vagS * curr[i + 1][j]) / diag;
								double newValue = oldValue + omega * (sum - oldValue);
								curr[i][j] = newValue;

								double r_i = diag * oldValue
									- c * vagE * curr[i][j + 1]
									- c * vagS * curr[i + 1][j]
									- prev[i][j];
								residualSq += r_i * r_i;
								continue;
							}

							if (i + 1 > H - 1) { // 7
								double diag = 1.0 + c * (vagE + vagN);
								double sum = (prev[i][j]
									+ c * vagE * curr[i][j + 1]
									+ c * vagN * curr[i - 1][j]) / diag;

								double newValue = oldValue + omega * (sum - oldValue);
								curr[i][j] = newValue;

								double r_i = diag * oldValue
									- c * vagE * curr[i][j + 1]
									- c * vagN * curr[i - 1][j]
									- prev[i][j];
								residualSq += r_i * r_i;
								continue;
							}

							double diag = 1.0 + c * (vagN + vagS + vagE);
							double sum = (prev[i][j]
								+ c * vagN * curr[i - 1][j]
								+ c * vagS * curr[i + 1][j]
								+ c * vagE * curr[i][j + 1]) / diag;
							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- c * vagN * curr[i - 1][j]
								- c * vagS * curr[i + 1][j]
								- c * vagE * curr[i][j + 1]
								- prev[i][j];
							residualSq += r_i * r_i;
							continue;
						}

						if (i + 1 > H - 1) { // 3
							if (j - 1 < 0) { // 7
								double diag = 1.0 + c * (vagE + vagN);
								double sum = (prev[i][j]
									+ c * vagE * curr[i][j + 1]
									+ c * vagN * curr[i - 1][j]) / diag;

								double newValue = oldValue + omega * (sum - oldValue);
								curr[i][j] = newValue;

								double r_i = diag * oldValue
									- c * vagE * curr[i][j + 1]
									- c * vagN * curr[i - 1][j]
									- prev[i][j];
								residualSq += r_i * r_i;
								continue;
							}

							if (j + 1 > W - 1) { // 6
								double diag = 1.0 + c * (vagW + vagN);
								double sum = (prev[i][j]
									+ c * vagW * curr[i][j - 1]
									+ c * vagN * curr[i - 1][j]) / diag;
								double newValue = oldValue + omega * (sum - oldValue);
								curr[i][j] = newValue;

								double r_i = diag * oldValue
									- c * vagW * curr[i][j - 1]
									- c * vagN * curr[i - 1][j]
									- prev[i][j];
								residualSq += r_i * r_i;
								continue;
							}

							double diag = 1.0 + c * (vagE + vagN + vagW);
							double sum = (prev[i][j]
								+ c * vagE * curr[i][j + 1]
								+ c * vagN * curr[i - 1][j]
								+ c * vagW * curr[i][j - 1]) / diag;
							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- c * vagE * curr[i][j + 1]
								- c * vagN * curr[i - 1][j]
								- c * vagW * curr[i][j - 1]
								- prev[i][j];
							residualSq += r_i * r_i;
							continue;
						}

						// vnútorný bod
						double diag = 1.0 + c * (vagW + vagS + vagE + vagN);
						double sum = (prev[i][j]
							+ c * vagW * curr[i][j - 1]
							+ c * vagS * curr[i + 1][j]
							+ c * vagE * curr[i][j + 1]
							+ c * vagN * curr[i - 1][j]) / diag;

						double newValue = oldValue + omega * (sum - oldValue);
						curr[i][j] = newValue;

						double r_i = diag * oldValue
							- c * vagW * curr[i][j - 1]
							- c * vagS * curr[i + 1][j]
							- c * vagE * curr[i][j + 1]
							- c * vagN * curr[i - 1][j]
							- prev[i][j];
						residualSq += r_i * r_i;
					}
				}

				double residual = sqrt(residualSq);
				if (residual < tol) {
					cout << residual << " " << iter << endl;
					break;
				}
			}

			next = curr;

			prev = next;

			finalChanPgm = im.to255(H, W, next);
			finalChanPgm1D = im.to1D(H, W, finalChanPgm);
			history[n][ch] = finalChanPgm1D;
		}

		finalChanPgm = im.to255(H, W, prev);
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

void ImageProcessing::сomputeDistantFunc(int H, int W, double tauD, vector<vector<double>>& phi, vector<vector<vector<double>>>& history, double Sx, double Sy, double r) {
	double tolForCirle = 0.5;
	double tol = 1e-4;

	vector<vector<double>> d(H, vector<double>(W, 0.0));
	vector<vector<bool>> F(H, vector<bool>(W, false));


	double n = H * W;
	int sizeF = 0;

	int maxIter = 10000;

	double h = 1.0;

	for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            double val = (i - Sx) * (i - Sx) + (j - Sy) * (j - Sy) - r * r;
            if (abs(val) <= tolForCirle) {
                d[i][j] = 0.0;
                F[i][j] = true;
                sizeF++;
            }
        }
    }

	history.clear();
	history.push_back(d);

	vector<vector<double>> dNew(H, vector<double>(W, 0.0));

	for (int k = 0; k < maxIter; k++) {

		

		double Dmx, Dpx, Dpy, Dmy;

		for (int i = 0; i < H; ++i) {
			for (int j = 0; j < W; ++j) {
				if (F[i][j]) 
					continue;		

				if (j > 0)
					Dmx = (d[i][j] - d[i][j - 1]) / h;
				else
					Dmx = 0;

				if (j < W - 1)
					Dpx = (d[i][j + 1] - d[i][j]) / h;
				else
					Dpx = 0;

				if (i > 0)
					Dpy = (d[i - 1][j] - d[i][j]) / h;
				else
					Dpy = 0;

				if (i < H - 1)
					Dmy = (d[i][j] - d[i + 1][j]) / h;
				else
					Dmy = 0;

				double maxDmx = max(Dmx, 0.0);
				double maxDmy = max(Dmy, 0.0);
				double minDpx = min(Dpx, 0.0);
				double minDpy = min(Dpy, 0.0);

				double deltaD = sqrt(maxDmx * maxDmx + maxDmy * maxDmy + minDpx * minDpx + minDpy * minDpy);

				dNew[i][j] = d[i][j] - tauD * (deltaD - 1);

				if (abs(dNew[i][j] - d[i][j]) < tol) {
					F[i][j] = true;
					sizeF++;
				}
			}
		}

		d = dNew;
		history.push_back(d);

		if (sizeF == n) {
			cout << "Distance function converged at iter " << k << "\n";
			break;
		}

		if (k == maxIter - 1) {
			cout << "WARNING: Distance function did NOT converge in " << maxIter << " iterations. Stabilized: " << sizeF << "/" << n << "\n";
		}
	}

	phi.assign(H, vector<double>(W, 0.0));

	for (int i = 0; i < H; ++i) {
		for (int j = 0; j < W; ++j) {
			double val = (i - Sx) * (i - Sx) + (j - Sy) * (j - Sy) - r * r;
			if (val < -tolForCirle) {
				phi[i][j] = -d[i][j];
			}
			else if (val > tolForCirle) {
				phi[i][j] = d[i][j];
			}
			else {
				phi[i][j] = 0.0;
			}
		}
	}
}

void ImageProcessing::segmentEdgeNormalMotion(ImageData& im, int N, vector<vector<vector<double>>>& history, double tau) {
	vector<vector<double>> dataOrigin = im.getData();
	int channelSize = dataOrigin.size();
	int H = im.getHeight();
	int W = im.getWidth();

	vector<double> channel;
	vector<vector<double>> finalChanPgm;
	vector<double> finalChanPgm1D;

	//double tau = Tau;
	double sigma = 0.25;
	double K = 500.0;
	int h = 1;

	history.assign(N, vector<vector<double>>(3, vector<double>(H * W, 0.0)));

	double tol = 1e-6;
	int    maxIter = 10000;
	double omega = 1.9;

	double c = tau / (h * h);
	double cSigma = sigma / (h * h);

	double tauD = 0.4;

	vector<vector<double>> phi;

	double sx = W / 2.0;
	double sy = H / 2.0;
	double r = 30;

	vector<vector<vector<double>>> zahlushka;

	сomputeDistantFunc(H, W, tauD, phi, zahlushka, sx, sy, r);

	cout << "ditance end" << endl;

	vector<vector<double>> uSigma(H, vector<double>(W, 0.0));


	channel = dataOrigin[0]; 
	vector<vector<double>> u0 = im.to2D(H, W, channel);

	for (int i = 0; i < H; ++i) {
		for (int j = 0; j < W; ++j) {

			if (j + 1 > W - 1) { //2
				if (i - 1 < 0) { //5
					uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * u0[i][j];
					uSigma[i][j] += cSigma * u0[i][j - 1] + cSigma * u0[i + 1][j];
					continue;
				}
				if (i + 1 > H - 1) { //6
					uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * u0[i][j];
					uSigma[i][j] += cSigma * u0[i][j - 1] + cSigma * u0[i - 1][j];
					continue;
				}
				uSigma[i][j] = (1 - ((3 * sigma) / (h * h))) * u0[i][j];
				uSigma[i][j] += cSigma * u0[i][j - 1] + cSigma * u0[i + 1][j] + cSigma * u0[i - 1][j];
				continue;
			}
			if (i - 1 < 0) { // 1
				if (j + 1 > W - 1) { // 5
					uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * u0[i][j];
					uSigma[i][j] += cSigma * u0[i][j - 1] + cSigma * u0[i + 1][j];
					continue;
				}
				if (j - 1 < 0) { //8
					uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * u0[i][j];
					uSigma[i][j] += cSigma * u0[i][j + 1] + cSigma * u0[i + 1][j];
					continue;
				}
				uSigma[i][j] = (1 - ((3 * sigma) / (h * h))) * u0[i][j];
				uSigma[i][j] += cSigma * u0[i][j - 1] + cSigma * u0[i + 1][j] + cSigma * u0[i][j + 1];
				continue;
			}
			if (j - 1 < 0) { //4
				if (i - 1 < 0) { //8
					uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * u0[i][j];
					uSigma[i][j] += cSigma * u0[i][j + 1] + cSigma * u0[i + 1][j];
					continue;
				}
				if (i + 1 > H - 1) { //7
					uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * u0[i][j];
					uSigma[i][j] += cSigma * u0[i][j + 1] + cSigma * u0[i - 1][j];
					continue;
				}
				uSigma[i][j] = (1 - ((3 * sigma) / (h * h))) * u0[i][j];
				uSigma[i][j] += cSigma * u0[i - 1][j] + cSigma * u0[i + 1][j] + cSigma * u0[i][j + 1];
				continue;
			}
			if (i + 1 > H - 1) { // 3
				if (j - 1 < 0) { // 7
					uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * u0[i][j];
					uSigma[i][j] += cSigma * u0[i][j + 1] + cSigma * u0[i - 1][j];
					continue;
				}
				if (j + 1 > W - 1) { //6
					uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * u0[i][j];
					uSigma[i][j] += cSigma * u0[i][j - 1] + cSigma * u0[i - 1][j];
					continue;
				}
				uSigma[i][j] = (1 - ((3 * sigma) / (h * h))) * u0[i][j];
				uSigma[i][j] += cSigma * u0[i][j + 1] + cSigma * u0[i - 1][j] + cSigma * u0[i][j - 1];
				continue;
			}
			uSigma[i][j] = (1 - ((4 * sigma) / (h * h))) * u0[i][j] + cSigma * u0[i][j - 1] + cSigma * u0[i + 1][j] + cSigma * u0[i][j + 1] + cSigma * u0[i - 1][j];
		}
	}

	cout << "uSigma end" << endl;

	int Hx = H + 2;
	int Wx = W + 2;
	vector<vector<double>> uSigmaBig(Hx, vector<double>(Wx, 0.0));

	for (int i = 0; i < H; ++i) { //inner
		for (int j = 0; j < W; ++j) {
			uSigmaBig[i + 1][j + 1] = uSigma[i][j];
		}
	}
	for (int j = 0; j < W; ++j) { //bot and top
		uSigmaBig[0][j + 1] = uSigma[0][j];
		uSigmaBig[Hx - 1][j + 1] = uSigma[H - 1][j];
	}
	for (int i = 0; i < H; ++i) { // left and right
		uSigmaBig[i + 1][0] = uSigma[i][0];
		uSigmaBig[i + 1][Wx - 1] = uSigma[i][W - 1];
	}

	uSigmaBig[0][0] = uSigma[0][0];
	uSigmaBig[0][Wx - 1] = uSigma[0][W - 1];
	uSigmaBig[Hx - 1][0] = uSigma[H - 1][0];
	uSigmaBig[Hx - 1][Wx - 1] = uSigma[H - 1][W - 1];

	vector<vector<double>> g(H, vector<double>(W, 0.0));

	for (int i = 0; i < H; ++i) {
		for (int j = 0; j < W; ++j) {

			int ii = i + 1;
			int jj = j + 1;

			double up = uSigmaBig[ii][jj];
			double uN = uSigmaBig[ii - 1][jj];
			double uS = uSigmaBig[ii + 1][jj];
			double uW = uSigmaBig[ii][jj - 1];
			double uE = uSigmaBig[ii][jj + 1];
			double uNE = uSigmaBig[ii - 1][jj + 1];
			double uNW = uSigmaBig[ii - 1][jj - 1];
			double uSE = uSigmaBig[ii + 1][jj + 1];
			double uSW = uSigmaBig[ii + 1][jj - 1];

			double gradUSigma0 = ((uN - uS) / (2 * h)) * (uN - uS) / (2 * h) + ((uE - uW) / (2 * h)) * (uE - uW) / (2 * h);

			g[i][j] = 1.0 / (1.0 + K * gradUSigma0);

		}
	}

	cout << "evolve start" << endl;

	vector<vector<double>> phiNew(H, vector<double>(W, 0.0));

	double isoTol = 3.0;

	for (int n = 0; n < N; ++n) {

		phiNew = phi; 

		double Dmx, Dpx, Dpy, Dmy;

		for (int i = 0; i < H; ++i) {
			for (int j = 0; j < W; ++j) {

				if (j > 0)
					Dmx = (phi[i][j] - phi[i][j - 1]) / h;
				else
					Dmx = 0;

				if (j < W - 1)
					Dpx = (phi[i][j + 1] - phi[i][j]) / h;
				else
					Dpx = 0;

				if (i > 0)
					Dpy = (phi[i - 1][j] - phi[i][j]) / h;
				else
					Dpy = 0;

				if (i < H - 1)
					Dmy = (phi[i][j] - phi[i + 1][j]) / h;
				else
					Dmy = 0;

				double maxDmx = max(Dmx, 0.0);
				double minDpx = min(Dpx, 0.0);
				double maxDmy = max(Dmy, 0.0);
				double minDpy = min(Dpy, 0.0);

				double gradPhi = sqrt(maxDmx * maxDmx + minDpx * minDpx + maxDmy * maxDmy + minDpy * minDpy);

				phiNew[i][j] = phi[i][j] - tau * g[i][j] * gradPhi;
			}
		}

		phi = phiNew;

		for (int i = 0; i < H; ++i) {
			for (int j = 0; j < W; ++j) {
				int idx = i * W + j;
				double bg = u0[i][j] * 255.0;

				bool onIso = false;
				double c0 = phi[i][j];

				if (j > 0 && c0 * phi[i][j - 1] < 0.0) onIso = true;
				if (j < W - 1 && c0 * phi[i][j + 1] < 0.0) onIso = true;
				if (i > 0 && c0 * phi[i - 1][j] < 0.0) onIso = true;
				if (i < H - 1 && c0 * phi[i + 1][j] < 0.0) onIso = true;
				if (c0 == 0.0) onIso = true;  

				if (onIso) {
					history[n][0][idx] = 255.0;   // R
					history[n][1][idx] = 0.0;     // G
					history[n][2][idx] = 0.0;     // B
				}
				else {
					history[n][0][idx] = bg;
					history[n][1][idx] = bg;
					history[n][2][idx] = bg;
				}
			}
		}

	}
}

void ImageProcessing::GAC(ImageData& im, int N, vector<vector<vector<double>>>& history, double tau, double cBalloon) {
	vector<vector<double>> dataOrigin = im.getData();
	int H = im.getHeight();
	int W = im.getWidth();

	double sigma = 0.25;
	double K = 500.0;
	int h = 1;

	double tol = 1e-6;
	int    maxIter = 10000;
	double omega = 1.9;

	double c = tau / (h * h);

	double cSigma = sigma / (h * h);

	double eps = 1e-3;

	double tauD = 0.4;
	double sx = W / 2.0;
	double sy = H / 2.0;
	double r = 30;

	history.assign(N, vector<vector<double>>(3, vector<double>(H * W, 0.0)));

	vector<double> channel = dataOrigin[0];
	vector<vector<double>> u0 = im.to2D(H, W, channel);

	vector<vector<double>> phi;
	vector<vector<vector<double>>> zahlushka;
	сomputeDistantFunc(H, W, tauD, phi, zahlushka, sx, sy, r);

	vector<vector<double>> uSigma(H, vector<double>(W, 0.0));

	for (int i = 0; i < H; ++i) {
		for (int j = 0; j < W; ++j) {

			if (j + 1 > W - 1) { //2
				if (i - 1 < 0) { //5
					uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * u0[i][j];
					uSigma[i][j] += cSigma * u0[i][j - 1] + cSigma * u0[i + 1][j];
					continue;
				}
				if (i + 1 > H - 1) { //6
					uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * u0[i][j];
					uSigma[i][j] += cSigma * u0[i][j - 1] + cSigma * u0[i - 1][j];
					continue;
				}
				uSigma[i][j] = (1 - ((3 * sigma) / (h * h))) * u0[i][j];
				uSigma[i][j] += cSigma * u0[i][j - 1] + cSigma * u0[i + 1][j] + cSigma * u0[i - 1][j];
				continue;
			}
			if (i - 1 < 0) { // 1
				if (j + 1 > W - 1) { // 5
					uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * u0[i][j];
					uSigma[i][j] += cSigma * u0[i][j - 1] + cSigma * u0[i + 1][j];
					continue;
				}
				if (j - 1 < 0) { //8
					uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * u0[i][j];
					uSigma[i][j] += cSigma * u0[i][j + 1] + cSigma * u0[i + 1][j];
					continue;
				}
				uSigma[i][j] = (1 - ((3 * sigma) / (h * h))) * u0[i][j];
				uSigma[i][j] += cSigma * u0[i][j - 1] + cSigma * u0[i + 1][j] + cSigma * u0[i][j + 1];
				continue;
			}
			if (j - 1 < 0) { //4
				if (i - 1 < 0) { //8
					uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * u0[i][j];
					uSigma[i][j] += cSigma * u0[i][j + 1] + cSigma * u0[i + 1][j];
					continue;
				}
				if (i + 1 > H - 1) { //7
					uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * u0[i][j];
					uSigma[i][j] += cSigma * u0[i][j + 1] + cSigma * u0[i - 1][j];
					continue;
				}
				uSigma[i][j] = (1 - ((3 * sigma) / (h * h))) * u0[i][j];
				uSigma[i][j] += cSigma * u0[i - 1][j] + cSigma * u0[i + 1][j] + cSigma * u0[i][j + 1];
				continue;
			}
			if (i + 1 > H - 1) { // 3
				if (j - 1 < 0) { // 7
					uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * u0[i][j];
					uSigma[i][j] += cSigma * u0[i][j + 1] + cSigma * u0[i - 1][j];
					continue;
				}
				if (j + 1 > W - 1) { //6
					uSigma[i][j] = (1 - ((2 * sigma) / (h * h))) * u0[i][j];
					uSigma[i][j] += cSigma * u0[i][j - 1] + cSigma * u0[i - 1][j];
					continue;
				}
				uSigma[i][j] = (1 - ((3 * sigma) / (h * h))) * u0[i][j];
				uSigma[i][j] += cSigma * u0[i][j + 1] + cSigma * u0[i - 1][j] + cSigma * u0[i][j - 1];
				continue;
			}
			uSigma[i][j] = (1 - ((4 * sigma) / (h * h))) * u0[i][j] + cSigma * u0[i][j - 1] + cSigma * u0[i + 1][j] + cSigma * u0[i][j + 1] + cSigma * u0[i - 1][j];
		}
	}

	int Hx = H + 2;
	int Wx = W + 2;
	vector<vector<double>> uSigmaBig(Hx, vector<double>(Wx, 0.0));

	for (int i = 0; i < H; ++i) { //inner
		for (int j = 0; j < W; ++j) {
			uSigmaBig[i + 1][j + 1] = uSigma[i][j];
		}
	}
	for (int j = 0; j < W; ++j) { //bot and top
		uSigmaBig[0][j + 1] = uSigma[0][j];
		uSigmaBig[Hx - 1][j + 1] = uSigma[H - 1][j];
	}
	for (int i = 0; i < H; ++i) { // left and right
		uSigmaBig[i + 1][0] = uSigma[i][0];
		uSigmaBig[i + 1][Wx - 1] = uSigma[i][W - 1];
	}

	uSigmaBig[0][0] = uSigma[0][0];
	uSigmaBig[0][Wx - 1] = uSigma[0][W - 1];
	uSigmaBig[Hx - 1][0] = uSigma[H - 1][0];
	uSigmaBig[Hx - 1][Wx - 1] = uSigma[H - 1][W - 1];


	vector<vector<double>> gPE(H, vector<double>(W, 0.0));
	vector<vector<double>> gPW(H, vector<double>(W, 0.0));
	vector<vector<double>> gPN(H, vector<double>(W, 0.0));
	vector<vector<double>> gPS(H, vector<double>(W, 0.0));
	vector<vector<double>> g0p(H, vector<double>(W, 0.0));

	for (int i = 0; i < H; ++i) {
		for (int j = 0; j < W; ++j) {

			int ii = i + 1;
			int jj = j + 1;

			double up = uSigmaBig[ii][jj];
			double uN = uSigmaBig[ii - 1][jj];
			double uS = uSigmaBig[ii + 1][jj];
			double uW = uSigmaBig[ii][jj - 1];
			double uE = uSigmaBig[ii][jj + 1];
			double uNE = uSigmaBig[ii - 1][jj + 1];
			double uNW = uSigmaBig[ii - 1][jj - 1];
			double uSE = uSigmaBig[ii + 1][jj + 1];
			double uSW = uSigmaBig[ii + 1][jj - 1];

			double gradPE = (((uE - up) / h) * ((uE - up) / h)) + (((uN + uNE - uS - uSE) / (4 * h)) * ((uN + uNE - uS - uSE) / (4 * h)));
			double gradPS = (((uS - up) / h) * ((uS - up) / h)) + (((uW + uSW - uE - uSE) / (4 * h)) * ((uW + uSW - uE - uSE) / (4 * h)));
			double gradPW = (((uW - up) / h) * ((uW - up) / h)) + (((uN + uNW - uS - uSW) / (4 * h)) * ((uN + uNW - uS - uSW) / (4 * h)));
			double gradPN = (((uN - up) / h) * ((uN - up) / h)) + (((uW + uNW - uE - uNE) / (4 * h)) * ((uW + uNW - uE - uNE) / (4 * h)));

			gPE[i][j] = 1.0 / (1.0 + K * gradPE);
			gPS[i][j] = 1.0 / (1.0 + K * gradPS);
			gPW[i][j] = 1.0 / (1.0 + K * gradPW);
			gPN[i][j] = 1.0 / (1.0 + K * gradPN);

			double gradUSigma0 = ((uN - uS) / (2 * h)) * (uN - uS) / (2 * h) + ((uE - uW) / (2 * h)) * (uE - uW) / (2 * h);

			g0p[i][j] = 1.0 / (1.0 + K * gradUSigma0);

		}
	}

	vector<vector<double>> prev = phi;   // phi^{n-1}

	for (int n = 0; n < N; n++) {
		vector<vector<double>> prevBig(Hx, vector<double>(Wx, 0.0));
		for (int i = 0; i < H; ++i)
			for (int j = 0; j < W; ++j)
				prevBig[i + 1][j + 1] = prev[i][j];
		for (int j = 0; j < W; ++j) {
			prevBig[0][j + 1] = prev[0][j];
			prevBig[Hx - 1][j + 1] = prev[H - 1][j];
		}
		for (int i = 0; i < H; ++i) {
			prevBig[i + 1][0] = prev[i][0];
			prevBig[i + 1][Wx - 1] = prev[i][W - 1];
		}
		prevBig[0][0] = prev[0][0];
		prevBig[0][Wx - 1] = prev[0][W - 1];
		prevBig[Hx - 1][0] = prev[H - 1][0];
		prevBig[Hx - 1][Wx - 1] = prev[H - 1][W - 1];

		vector<vector<double>> gradPEEps(H, vector<double>(W, 0.0));
		vector<vector<double>> gradPWEps(H, vector<double>(W, 0.0));
		vector<vector<double>> gradPNEps(H, vector<double>(W, 0.0));
		vector<vector<double>> gradPSEps(H, vector<double>(W, 0.0));

		for (int i = 0; i < H; ++i) {
			for (int j = 0; j < W; ++j) {

				int ii = i + 1;
				int jj = j + 1;

				double up = prevBig[ii][jj];
				double uN = prevBig[ii - 1][jj];
				double uS = prevBig[ii + 1][jj];
				double uW = prevBig[ii][jj - 1];
				double uE = prevBig[ii][jj + 1];
				double uNE = prevBig[ii - 1][jj + 1];
				double uNW = prevBig[ii - 1][jj - 1];
				double uSE = prevBig[ii + 1][jj + 1];
				double uSW = prevBig[ii + 1][jj - 1];

				double gradPE = (((uE - up) / h) * ((uE - up) / h)) + (((uN + uNE - uS - uSE) / (4 * h)) * ((uN + uNE - uS - uSE) / (4 * h)));
				double gradPS = (((uS - up) / h) * ((uS - up) / h)) + (((uW + uSW - uE - uSE) / (4 * h)) * ((uW + uSW - uE - uSE) / (4 * h)));
				double gradPW = (((uW - up) / h) * ((uW - up) / h)) + (((uN + uNW - uS - uSW) / (4 * h)) * ((uN + uNW - uS - uSW) / (4 * h)));
				double gradPN = (((uN - up) / h) * ((uN - up) / h)) + (((uW + uNW - uE - uNE) / (4 * h)) * ((uW + uNW - uE - uNE) / (4 * h)));

				gradPEEps[i][j] = sqrt((eps * eps) + gradPE);
				gradPSEps[i][j] = sqrt((eps * eps) + gradPS);
				gradPWEps[i][j] = sqrt((eps * eps) + gradPW);
				gradPNEps[i][j] = sqrt((eps * eps) + gradPN);
			}
		}

		vector<vector<double>> gradEpsAvg(H, vector<double>(W, 0.0));
		for (int i = 0; i < H; ++i) {
			for (int j = 0; j < W; ++j) {
				double sum = 0.0;
				double count = 0.0;
				if (i - 1 >= 0) { sum += gradPNEps[i][j]; count += 1.0; }
				if (i + 1 <= H - 1) { sum += gradPSEps[i][j]; count += 1.0; }
				if (j - 1 >= 0) { sum += gradPWEps[i][j]; count += 1.0; }
				if (j + 1 <= W - 1) { sum += gradPEEps[i][j]; count += 1.0; }
				gradEpsAvg[i][j] = sum / count;
			}
		}

		vector<vector<double>> curr = prev;

		for (int iter = 0; iter < maxIter; iter++) {
			double residualSq = 0.0;

			for (int i = 0; i < H; ++i) {
				for (int j = 0; j < W; ++j) {

					double cc = tau / (h * h);

					double oldValue = curr[i][j];

					double gn = gPN[i][j];
					double gs = gPS[i][j];
					double gw = gPW[i][j];
					double ge = gPE[i][j];

					double gradN = gradPNEps[i][j];
					double gradS = gradPSEps[i][j];
					double gradW = gradPWEps[i][j];
					double gradE = gradPEEps[i][j];

					double vagN = gn / gradN;
					double vagS = gs / gradS;
					double vagW = gw / gradW;
					double vagE = ge / gradE;

					double gradAvg = gradEpsAvg[i][j];
					cc *= gradAvg;

					double rhs = prev[i][j] - tau * gradAvg * cBalloon * g0p[i][j];

					if (j + 1 > W - 1) { // 2
						if (i - 1 < 0) { // 5
							double diag = 1.0 + cc * (vagS + vagW);
							double sum = (rhs
								+ cc * vagW * curr[i][j - 1]
								+ cc * vagS * curr[i + 1][j]) / diag;

							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- cc * vagW * curr[i][j - 1]
								- cc * vagS * curr[i + 1][j]
								- rhs;
							residualSq += r_i * r_i;
							continue;
						}

						if (i + 1 > H - 1) { // 6
							double diag = 1.0 + cc * (vagW + vagN);
							double sum = (rhs
								+ cc * vagW * curr[i][j - 1]
								+ cc * vagN * curr[i - 1][j]) / diag;

							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- cc * vagW * curr[i][j - 1]
								- cc * vagN * curr[i - 1][j]
								- rhs;
							residualSq += r_i * r_i;
							continue;
						}

						double diag = 1.0 + cc * (vagS + vagW + vagN);
						double sum = (rhs
							+ cc * vagW * curr[i][j - 1]
							+ cc * vagS * curr[i + 1][j]
							+ cc * vagN * curr[i - 1][j]) / diag;

						double newValue = oldValue + omega * (sum - oldValue);
						curr[i][j] = newValue;

						double r_i = diag * oldValue
							- cc * vagW * curr[i][j - 1]
							- cc * vagS * curr[i + 1][j]
							- cc * vagN * curr[i - 1][j]
							- rhs;
						residualSq += r_i * r_i;
						continue;
					}

					if (i - 1 < 0) { // 1
						if (j + 1 > W - 1) { // 5
							double diag = 1.0 + cc * (vagS + vagW);
							double sum = (rhs
								+ cc * vagW * curr[i][j - 1]
								+ cc * vagS * curr[i + 1][j]) / diag;

							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- cc * vagW * curr[i][j - 1]
								- cc * vagS * curr[i + 1][j]
								- rhs;
							residualSq += r_i * r_i;
							continue;
						}

						if (j - 1 < 0) { // 8
							double diag = 1.0 + cc * (vagS + vagE);
							double sum = (rhs
								+ cc * vagE * curr[i][j + 1]
								+ cc * vagS * curr[i + 1][j]) / diag;

							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- cc * vagE * curr[i][j + 1]
								- cc * vagS * curr[i + 1][j]
								- rhs;
							residualSq += r_i * r_i;
							continue;
						}

						double diag = 1.0 + cc * (vagS + vagW + vagE);
						double sum = (rhs
							+ cc * vagW * curr[i][j - 1]
							+ cc * vagS * curr[i + 1][j]
							+ cc * vagE * curr[i][j + 1]) / diag;

						double newValue = oldValue + omega * (sum - oldValue);
						curr[i][j] = newValue;

						double r_i = diag * oldValue
							- cc * vagW * curr[i][j - 1]
							- cc * vagS * curr[i + 1][j]
							- cc * vagE * curr[i][j + 1]
							- rhs;
						residualSq += r_i * r_i;
						continue;
					}

					if (j - 1 < 0) { // 4
						if (i - 1 < 0) { // 8
							double diag = 1.0 + cc * (vagS + vagE);
							double sum = (rhs
								+ cc * vagE * curr[i][j + 1]
								+ cc * vagS * curr[i + 1][j]) / diag;

							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- cc * vagE * curr[i][j + 1]
								- cc * vagS * curr[i + 1][j]
								- rhs;
							residualSq += r_i * r_i;
							continue;
						}

						if (i + 1 > H - 1) { // 7
							double diag = 1.0 + cc * (vagE + vagN);
							double sum = (rhs
								+ cc * vagE * curr[i][j + 1]
								+ cc * vagN * curr[i - 1][j]) / diag;

							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- cc * vagE * curr[i][j + 1]
								- cc * vagN * curr[i - 1][j]
								- rhs;
							residualSq += r_i * r_i;
							continue;
						}

						double diag = 1.0 + cc * (vagN + vagS + vagE);
						double sum = (rhs
							+ cc * vagN * curr[i - 1][j]
							+ cc * vagS * curr[i + 1][j]
							+ cc * vagE * curr[i][j + 1]) / diag;

						double newValue = oldValue + omega * (sum - oldValue);
						curr[i][j] = newValue;

						double r_i = diag * oldValue
							- cc * vagN * curr[i - 1][j]
							- cc * vagS * curr[i + 1][j]
							- cc * vagE * curr[i][j + 1]
							- rhs;
						residualSq += r_i * r_i;
						continue;
					}

					if (i + 1 > H - 1) { // 3
						if (j - 1 < 0) { // 7
							double diag = 1.0 + cc * (vagE + vagN);
							double sum = (rhs
								+ cc * vagE * curr[i][j + 1]
								+ cc * vagN * curr[i - 1][j]) / diag;

							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- cc * vagE * curr[i][j + 1]
								- cc * vagN * curr[i - 1][j]
								- rhs;
							residualSq += r_i * r_i;
							continue;
						}

						if (j + 1 > W - 1) { // 6
							double diag = 1.0 + cc * (vagW + vagN);
							double sum = (rhs
								+ cc * vagW * curr[i][j - 1]
								+ cc * vagN * curr[i - 1][j]) / diag;

							double newValue = oldValue + omega * (sum - oldValue);
							curr[i][j] = newValue;

							double r_i = diag * oldValue
								- cc * vagW * curr[i][j - 1]
								- cc * vagN * curr[i - 1][j]
								- rhs;
							residualSq += r_i * r_i;
							continue;
						}

						double diag = 1.0 + cc * (vagE + vagN + vagW);
						double sum = (rhs
							+ cc * vagE * curr[i][j + 1]
							+ cc * vagN * curr[i - 1][j]
							+ cc * vagW * curr[i][j - 1]) / diag;

						double newValue = oldValue + omega * (sum - oldValue);
						curr[i][j] = newValue;

						double r_i = diag * oldValue
							- cc * vagE * curr[i][j + 1]
							- cc * vagN * curr[i - 1][j]
							- cc * vagW * curr[i][j - 1]
							- rhs;
						residualSq += r_i * r_i;
						continue;
					}

					// vnútorný bod
					double diag = 1.0 + cc * (vagW + vagS + vagE + vagN);
					double sum = (rhs
						+ cc * vagW * curr[i][j - 1]
						+ cc * vagS * curr[i + 1][j]
						+ cc * vagE * curr[i][j + 1]
						+ cc * vagN * curr[i - 1][j]) / diag;

					double newValue = oldValue + omega * (sum - oldValue);
					curr[i][j] = newValue;

					double r_i = diag * oldValue
						- cc * vagW * curr[i][j - 1]
						- cc * vagS * curr[i + 1][j]
						- cc * vagE * curr[i][j + 1]
						- cc * vagN * curr[i - 1][j]
						- rhs;
					residualSq += r_i * r_i;
				}
			}

			double residual = sqrt(residualSq);
			if (residual < tol) break;
		}

		prev = curr; 

		for (int i = 0; i < H; ++i) {
			for (int j = 0; j < W; ++j) {
				int idx = i * W + j;
				double bg = u0[i][j] * 255.0;

				bool onIso = false;
				double c0 = prev[i][j];
				if (j > 0 && c0 * prev[i][j - 1] < 0.0) onIso = true;
				if (j < W - 1 && c0 * prev[i][j + 1] < 0.0) onIso = true;
				if (i > 0 && c0 * prev[i - 1][j] < 0.0) onIso = true;
				if (i < H - 1 && c0 * prev[i + 1][j] < 0.0) onIso = true;
				if (c0 == 0.0) onIso = true;

				if (onIso) {
					history[n][0][idx] = 255.0;
					history[n][1][idx] = 0.0;
					history[n][2][idx] = 0.0;
				}
				else {
					history[n][0][idx] = bg;
					history[n][1][idx] = bg;
					history[n][2][idx] = bg;
				}
			}
		}
	}
}