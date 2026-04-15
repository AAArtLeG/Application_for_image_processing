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
	numOfItersForLinDif = QInputDialog::getInt(
		this,
		"Input",
		"Enter a number:",
		1,      // value by default
		1,      // min
		1000,    // max
		1,      // step
		&ok
	);

	if (ok) {
		// user pressed OK
		qDebug() << "Entered value:" << numOfItersForLinDif;
	}
	else {
		// user pressed Cancel
		qDebug() << "Cancelled";
		return;
	}

	ui->spinBoxLinDifIters->setRange(0, numOfItersForLinDif - 1);

	//ip.linDiffusionExplicite(images[0], numOfItersForLinDif, history);
	//ip.linDiffusionImplicit(images[0], numOfItersForLinDif, history);
	ip.nonLinPeronaMalikSemiImplicit(images[0], numOfItersForLinDif, history);
	//vW->setImage(images[0].toQImageGray());

	cout << "history size = " << history.size() << "\n";

	ui->spinBoxLinDifIters->setValue(numOfItersForLinDif - 1);

	ui->spinBoxLinDifIters->setEnabled(true);
	ui->pushButtonSelectCur->setEnabled(true);
	ui->pushButtonStopOnLast->setEnabled(true);
}

void ImageViewer::on_pushButtonSelectCur_clicked() {
	images[0].setData(history[curIterOfLinDif]);

	vW->setImage(images[0].toQImageGray());

	numOfItersForLinDif = 0;
	curIterOfLinDif = 0;
	ui->spinBoxLinDifIters->blockSignals(true);
	ui->spinBoxLinDifIters->setRange(0, 0);
	ui->spinBoxLinDifIters->blockSignals(false);
	ui->spinBoxLinDifIters->setEnabled(false);
	ui->pushButtonSelectCur->setEnabled(false);
	ui->pushButtonStopOnLast->setEnabled(false);
}

void ImageViewer::on_pushButtonStopOnLast_clicked() {
	images[0].setData(history[numOfItersForLinDif - 1]);

	vW->setImage(images[0].toQImageGray());

	numOfItersForLinDif = 0;
	curIterOfLinDif = 0;
	ui->spinBoxLinDifIters->blockSignals(true);
	ui->spinBoxLinDifIters->setRange(0, 0);
	ui->spinBoxLinDifIters->blockSignals(false);
	ui->spinBoxLinDifIters->setEnabled(false);
	ui->pushButtonSelectCur->setEnabled(false);
	ui->pushButtonStopOnLast->setEnabled(false);
}

void ImageViewer::on_spinBoxLinDifIters_valueChanged(int value)
{
	qDebug() << value;
	curIterOfLinDif = value;

	images[0].setData(history[curIterOfLinDif]);

	vW->setImage(images[0].toQImageGray());
}