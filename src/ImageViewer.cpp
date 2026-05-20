#include "ImageViewer.h"

using namespace std;

ImageViewer::ImageViewer(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::ImageViewerClass)
{
	ui->setupUi(this);
	vW = new ViewerWidget(QSize(0, 0), ui->scrollArea);
	ui->scrollArea->setWidget(vW);

	ui->scrollArea->setBackgroundRole(QPalette::Dark);
	ui->scrollArea->setWidgetResizable(false);
	ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	vW->setObjectName("ViewerWidget");
}

//ImageViewer Events
void ImageViewer::closeEvent(QCloseEvent* event)
{
	if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation", "Are you sure you want to exit?", QMessageBox::Yes | QMessageBox::No))
	{
		event->accept();
	}
	else {
		event->ignore();
	}
}

//Image functions
bool ImageViewer::openImage(QString filename)
{
	QImage loadedImg(filename);
	if (!loadedImg.isNull()) {
		images.clear();

		vW->setImage(loadedImg);
		const QImage* image = vW->getImage();
		ImageData data = ImageData(image->width(), image->height(), vW->getData(), image->bytesPerLine(), image->depth());
		images.push_back(data);

		//Debug

		/*QFileInfo fi(filename);
		const QString base = fi.completeBaseName();
		for (int c = 0; c < data.getData().size(); c++)
		{
			QString outName = base + QString("_ch%1.pgm").arg(c);
			saveToPgm(outName.toStdString(), data.getWidth(), data.getHeight(), data.getData(c));
		}*/

		return true;
	}
	return false;
}
bool ImageViewer::saveImage(QString filename)
{
	const QImage* img = vW->getImage();
	if (!img || img->isNull()) return false;

	QFileInfo fi(filename);
	QString extension = fi.suffix().toLower();

	if (extension.isEmpty()) {
		extension = "jpg";
		filename += ".jpg";
	}

	return img->save(filename, extension.toUtf8().constData());
}

//Slots
void ImageViewer::on_actionOpen_triggered()
{
	QString folder = settings.value("folder_img_load_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.xbm *.xpm);;All files (*)";
	QString fileName = QFileDialog::getOpenFileName(this, "Load image", folder, fileFilter);
	if (fileName.isEmpty()) { return; }

	QFileInfo fi(fileName);
	settings.setValue("folder_img_load_path", fi.absoluteDir().absolutePath());

	if (!openImage(fileName)) {
		msgBox.setText("Unable to open image.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
}
void ImageViewer::on_actionSave_as_triggered()
{
	QString folder = settings.value("folder_img_save_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.xbm *.xpm);;All files (*)";
	QString fileName = QFileDialog::getSaveFileName(this, "Save image", folder, fileFilter);
	if (!fileName.isEmpty()) {
		QFileInfo fi(fileName);
		settings.setValue("folder_img_save_path", fi.absoluteDir().absolutePath());

		if (!saveImage(fileName)) {
			msgBox.setText("Unable to save image.");
			msgBox.setIcon(QMessageBox::Warning);
		}
		else {
			msgBox.setText(QString("File %1 saved.").arg(fileName));
			msgBox.setIcon(QMessageBox::Information);
		}
		msgBox.exec();
	}
}
void ImageViewer::on_actionExit_triggered()
{
	this->close();
}

void ImageViewer::on_pushButtonFSHS_clicked() {
	std::cout << "click" << std::endl;

	std::cout << "images size = " << images.size() << "\n";
	if (images.isEmpty()) return;
	std::cout << "img w/h = " << images[0].getWidth() << " " << images[0].getHeight() << "\n";

	std::vector<double>& channel0 = images[0].getData(0);
	std::cout << "before: " << channel0[0] << " " << channel0[1] << " " << channel0[2] << "\n";

	images[0].fshs(256);

	std::cout << "after:  " << channel0[0] << " " << channel0[1] << " " << channel0[2] << "\n";

	vW->setImage(images[0].toQImageGray());

	std::cout << "images size = " << images.size() << "\n";
	if (images.isEmpty()) return;
	std::cout << "img w/h = " << images[0].getWidth() << " " << images[0].getHeight() << "\n";

	std::cout << "clicked" << std::endl;
}

void ImageViewer::on_pushButtonCon_clicked() {
	images[0].convolution();

	vW->setImage(images[0].toQImageGray());
}

void ImageViewer::on_pushButtonLinDif_clicked() {
	bool ok;

	double tau = QInputDialog::getDouble(
		this,
		"Input",
		"Enter tau:",
		0.2,    // value by default
		0.0,    // min
		1000.0, // max
		2,      // decimals
		&ok
	);

	if (ok) {
		qDebug() << "Entered tau:" << tau;
	}
	else {
		qDebug() << "Cancelled";
		return;
	}

	if (tau < 0.5) {
		numOfItersForLinDif = QInputDialog::getInt(
			this,
			"Input",
			"Enter a number:",
			1,      // value by default
			1,      // min
			1000,   // max
			1,      // step
			&ok
		);

		if (ok) {
			qDebug() << "Entered value:" << numOfItersForLinDif;
		}
		else {
			qDebug() << "Cancelled";
			return;
		}

		ui->spinBoxLinDifIters->setRange(0, numOfItersForLinDif - 1);

		ip.linDiffusionExplicite(images[0], numOfItersForLinDif, history, tau);
	}
	else {
		numOfItersForLinDif = 1;

		ui->spinBoxLinDifIters->setRange(0, numOfItersForLinDif - 1);

		ip.linDiffusionImplicit(images[0], numOfItersForLinDif, history, tau);
	}

	//cout << "history size = " << history.size() << "\n";

	ui->spinBoxLinDifIters->setValue(numOfItersForLinDif - 1);

	images[0].setData(history[numOfItersForLinDif - 1]);
	vW->setImage(images[0].toQImageGray());

	ui->spinBoxLinDifIters->setEnabled(true);
	ui->pushButtonSelectCur->setEnabled(true);
	ui->pushButtonStopOnLast->setEnabled(true);
}

void ImageViewer::on_pushButtonPeronMalik_clicked() {
	bool ok;

	double tau = QInputDialog::getDouble(
		this,
		"Input",
		"Enter tau:",
		0.2,    // value by default
		0.0,    // min
		1000.0, // max
		2,      // decimals
		&ok
	);

	if (ok) {
		qDebug() << "Entered tau:" << tau;
	}
	else {
		qDebug() << "Cancelled";
		return;
	}

	numOfItersForLinDif = QInputDialog::getInt(
		this,
		"Input",
		"Enter a number:",
		1,      // value by default
		1,      // min
		1000,   // max
		1,      // step
		&ok
	);

	if (ok) {
		qDebug() << "Entered value:" << numOfItersForLinDif;
	}
	else {
		qDebug() << "Cancelled";
		return;
	}

	ui->spinBoxLinDifIters->setRange(0, numOfItersForLinDif - 1);

	ip.nonLinPeronaMalikSemiImplicit(images[0], numOfItersForLinDif, history, tau);
	//ip.GMCF(images[0], numOfItersForLinDif, history, tau);

	ui->spinBoxLinDifIters->setValue(numOfItersForLinDif - 1);

	images[0].setData(history[numOfItersForLinDif - 1]);
	vW->setImage(images[0].toQImageGray());

	ui->spinBoxLinDifIters->setEnabled(true);
	ui->pushButtonSelectCur->setEnabled(true);
	ui->pushButtonStopOnLast->setEnabled(true);
}

void ImageViewer::on_pushButtonGMFC_clicked() {
	bool ok;

	double tau = QInputDialog::getDouble(
		this,
		"Input",
		"Enter tau:",
		0.2,    // value by default
		0.0,    // min
		1000.0, // max
		2,      // decimals
		&ok
	);

	if (ok) {
		qDebug() << "Entered tau:" << tau;
	}
	else {
		qDebug() << "Cancelled";
		return;
	}

	numOfItersForLinDif = QInputDialog::getInt(
		this,
		"Input",
		"Enter a number:",
		1,      // value by default
		1,      // min
		1000,   // max
		1,      // step
		&ok
	);

	if (ok) {
		qDebug() << "Entered value:" << numOfItersForLinDif;
	}
	else {
		qDebug() << "Cancelled";
		return;
	}

	ui->spinBoxLinDifIters->setRange(0, numOfItersForLinDif - 1);

	//ip.nonLinPeronaMalikSemiImplicit(images[0], numOfItersForLinDif, history, tau);
	ip.GMCF(images[0], numOfItersForLinDif, history, tau);

	ui->spinBoxLinDifIters->setValue(numOfItersForLinDif - 1);

	images[0].setData(history[numOfItersForLinDif - 1]);
	vW->setImage(images[0].toQImageGray());

	ui->spinBoxLinDifIters->setEnabled(true);
	ui->pushButtonSelectCur->setEnabled(true);
	ui->pushButtonStopOnLast->setEnabled(true);
}

//void ImageViewer::on_pushButtonSelectCur_clicked() {
//	images[0].setData(history[curIterOfLinDif]);
//
//	vW->setImage(images[0].toQImageGray());
//
//	numOfItersForLinDif = 0;
//	curIterOfLinDif = 0;
//	ui->spinBoxLinDifIters->blockSignals(true);
//	ui->spinBoxLinDifIters->setRange(0, 0);
//	ui->spinBoxLinDifIters->blockSignals(false);
//	ui->spinBoxLinDifIters->setEnabled(false);
//	ui->pushButtonSelectCur->setEnabled(false);
//	ui->pushButtonStopOnLast->setEnabled(false);
//}

void ImageViewer::on_pushButtonSelectCur_clicked() {

	if (segMode) {
		showSegFrame(curIterOfLinDif);
		segMode = false;
	}
	else {
		images[0].setData(history[curIterOfLinDif]);
		vW->setImage(images[0].toQImageGray());
	}

	numOfItersForLinDif = 0;
	curIterOfLinDif = 0;
	ui->spinBoxLinDifIters->blockSignals(true);
	ui->spinBoxLinDifIters->setRange(0, 0);
	ui->spinBoxLinDifIters->blockSignals(false);
	ui->spinBoxLinDifIters->setEnabled(false);
	ui->pushButtonSelectCur->setEnabled(false);
	ui->pushButtonStopOnLast->setEnabled(false);
}

//void ImageViewer::on_pushButtonStopOnLast_clicked() {
//	images[0].setData(history[numOfItersForLinDif - 1]);
//
//	vW->setImage(images[0].toQImageGray());
//
//	numOfItersForLinDif = 0;
//	curIterOfLinDif = 0;
//	ui->spinBoxLinDifIters->blockSignals(true);
//	ui->spinBoxLinDifIters->setRange(0, 0);
//	ui->spinBoxLinDifIters->blockSignals(false);
//	ui->spinBoxLinDifIters->setEnabled(false);
//	ui->pushButtonSelectCur->setEnabled(false);
//	ui->pushButtonStopOnLast->setEnabled(false);
//}

void ImageViewer::on_pushButtonStopOnLast_clicked() {

	if (segMode) {
		showSegFrame((int)history.size() - 1);
		segMode = false;
	}
	else {
		images[0].setData(history[numOfItersForLinDif - 1]);
		vW->setImage(images[0].toQImageGray());
	}

	numOfItersForLinDif = 0;
	curIterOfLinDif = 0;
	ui->spinBoxLinDifIters->blockSignals(true);
	ui->spinBoxLinDifIters->setRange(0, 0);
	ui->spinBoxLinDifIters->blockSignals(false);
	ui->spinBoxLinDifIters->setEnabled(false);
	ui->pushButtonSelectCur->setEnabled(false);
	ui->pushButtonStopOnLast->setEnabled(false);
}

//void ImageViewer::on_spinBoxLinDifIters_valueChanged(int value)
//{
//	qDebug() << value;
//	curIterOfLinDif = value;
//
//	images[0].setData(history[curIterOfLinDif]);
//
//	vW->setImage(images[0].toQImageGray());
//}

void ImageViewer::on_spinBoxLinDifIters_valueChanged(int value) {
	qDebug() << value;
	curIterOfLinDif = value;

	if (segMode) {
		showSegFrame(curIterOfLinDif);
	}
	else {
		images[0].setData(history[curIterOfLinDif]);
		vW->setImage(images[0].toQImageGray());
	}
}

QImage ImageViewer::phiToGrayscale(const vector<vector<double>>& phi) {
	int H = phi.size();
	int W = phi[0].size();

	// Найти min/max
	double mn = phi[0][0], mx = phi[0][0];
	for (int i = 0; i < H; ++i)
		for (int j = 0; j < W; ++j) {
			mn = min(mn, phi[i][j]);
			mx = max(mx, phi[i][j]);
		}

	double range = mx - mn;
	if (range < 1e-12) range = 1.0;

	QImage img(W, H, QImage::Format_Grayscale8);
	for (int i = 0; i < H; ++i)
		for (int j = 0; j < W; ++j) {
			int g = (int)(255.0 * (phi[i][j] - mn) / range);
			img.setPixel(j, i, qRgb(g, g, g));
		}

	cout << "WW" << endl;

	return img;
}

void ImageViewer::on_pushButtonDistanceFunc_clicked() {
	bool ok;

	double Sx = QInputDialog::getDouble(this, "Distance", "Sx:", 50, 0, 10000, 2, &ok);
	if (!ok) return;
	double Sy = QInputDialog::getDouble(this, "Distance", "Sy:", 50, 0, 10000, 2, &ok);
	if (!ok) return;
	double r = QInputDialog::getDouble(this, "Distance", "r:", 10, 0.1, 10000, 2, &ok);
	if (!ok) return;
	double tauD = QInputDialog::getDouble(this, "Distance", "tauD:", 0.25, 0, 0.5, 3, &ok);
	if (!ok) return;

	int H = images[0].getHeight();
	int W = images[0].getWidth();

	vector<vector<double>> phi;
	history.clear();

	ip.сomputeDistantFunc(H, W, tauD, phi, history, Sx, Sy, r);


	double maxNeg = 0.0;   
	double maxPos = 0.0; 
	for (int i = 0; i < H; ++i)
		for (int j = 0; j < W; ++j) {
			if (phi[i][j] < 0 && -phi[i][j] > maxNeg) maxNeg = -phi[i][j];
			if (phi[i][j] > 0 && phi[i][j] > maxPos) maxPos = phi[i][j];
		}
	if (maxNeg < 1e-12) maxNeg = 1.0;
	if (maxPos < 1e-12) maxPos = 1.0;

	QImage out(W, H, QImage::Format_RGB888);
	for (int i = 0; i < H; ++i) {
		for (int j = 0; j < W; ++j) {

			double v = phi[i][j];
			int R, G, B;

			if (v < -0.5) {
				int t = (int)(255.0 * (-v) / maxNeg);
				if (t > 255) t = 255;
				R = 0;
				G = 0;
				B = 255 - t / 2;          
			}
			else if (v > 0.5) {
				int t = (int)(255.0 * v / maxPos);
				if (t > 255) t = 255;
				R = t;
				G = 255 - t;
				B = 0;
			}
			else {
				R = 255;
				G = 255;
				B = 255;
			}

			out.setPixel(j, i, qRgb(R, G, B));   
		}
	}

	vW->setImage(out);
}

//void ImageViewer::on_pushButtonEdgeMotion_clicked() {
//
//	double tau = 0.5;
//	int    N = 300;
//
//	qDebug() << "Segment test: tau =" << tau << " N =" << N;
//
//	ip.segmentEdgeNormalMotion(images[0], N, history, tau);
//
//	int H = images[0].getHeight();
//	int W = images[0].getWidth();
//	int idx = N - 1;   
//
//	QImage out(W, H, QImage::Format_RGB888);
//	for (int i = 0; i < H; ++i) {
//		for (int j = 0; j < W; ++j) {
//			int p = i * W + j;
//			int R = (int)history[idx][0][p];
//			int G = (int)history[idx][1][p];
//			int B = (int)history[idx][2][p];
//			if (R < 0) R = 0; if (R > 255) R = 255;
//			if (G < 0) G = 0; if (G > 255) G = 255;
//			if (B < 0) B = 0; if (B > 255) B = 255;
//			out.setPixel(j, i, qRgb(R, G, B));
//		}
//	}
//
//	vW->setImage(out);
//}

void ImageViewer::showSegFrame(int k) {
	if (history.empty() || k < 0 || k >= (int)history.size()) return;

	int H = images[0].getHeight();
	int W = images[0].getWidth();

	QImage out(W, H, QImage::Format_RGB888);
	for (int i = 0; i < H; ++i) {
		for (int j = 0; j < W; ++j) {
			int p = i * W + j;
			int R = (int)history[k][0][p];
			int G = (int)history[k][1][p];
			int B = (int)history[k][2][p];
			if (R < 0) R = 0; if (R > 255) R = 255;
			if (G < 0) G = 0; if (G > 255) G = 255;
			if (B < 0) B = 0; if (B > 255) B = 255;
			out.setPixel(j, i, qRgb(R, G, B));
		}
	}

	vW->setImage(out);
}

void ImageViewer::on_pushButtonEdgeMotion_clicked() {

	double tau = 0.5;
	int    N = 300;

	ip.segmentEdgeNormalMotion(images[0], N, history, tau);

	segMode = true;

	ui->spinBoxLinDifIters->blockSignals(true);
	ui->spinBoxLinDifIters->setRange(0, N - 1);
	ui->spinBoxLinDifIters->setValue(N - 1);
	ui->spinBoxLinDifIters->blockSignals(false);

	showSegFrame(N - 1);

	ui->spinBoxLinDifIters->setEnabled(true);
	ui->pushButtonSelectCur->setEnabled(true);
	ui->pushButtonStopOnLast->setEnabled(true);
}

void ImageViewer::on_pushButtonGAC_clicked() {

	double tau = 0.5;
	int    N = 400;
	double cBalloon = 1.0;

	ip.GAC(images[0], N, history, tau, cBalloon);

	segMode = true;

	ui->spinBoxLinDifIters->blockSignals(true);
	ui->spinBoxLinDifIters->setRange(0, N - 1);
	ui->spinBoxLinDifIters->setValue(N - 1);
	ui->spinBoxLinDifIters->blockSignals(false);

	showSegFrame(N - 1);

	ui->spinBoxLinDifIters->setEnabled(true);
	ui->pushButtonSelectCur->setEnabled(true);
	ui->pushButtonStopOnLast->setEnabled(true);
}





