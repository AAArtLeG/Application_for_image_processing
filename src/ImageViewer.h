#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets>
#include "ui_ImageViewer.h"
#include "ViewerWidget.h"
#include "ImageProcessing.h"

using namespace std;

class ImageViewer : public QMainWindow
{
	Q_OBJECT

public:
	ImageViewer(QWidget* parent = Q_NULLPTR);
	~ImageViewer() { delete ui; }
private:
	Ui::ImageViewerClass* ui;
	ViewerWidget* vW;

	QSettings settings;
	QMessageBox msgBox;

	QVector<ImageData> images;

	//ImageViewer Events
	void closeEvent(QCloseEvent* event);

	//Image functions
	bool openImage(QString filename);
	bool saveImage(QString filename);

	ImageProcessing ip;

	int numOfItersForLinDif = 0;
	int curIterOfLinDif = 0;
	vector<vector<vector<double>>> history = vector<vector<vector<double>>>{};

	QImage phiToGrayscale(const vector<vector<double>>& phi);

	void showSegFrame(int k);
	bool segMode = false;
private slots:
	void on_actionOpen_triggered();
	void on_actionSave_as_triggered();
	void on_actionExit_triggered();
	void on_pushButtonFSHS_clicked(); 
	void on_pushButtonCon_clicked(); 
	void on_pushButtonLinDif_clicked();
	void on_pushButtonPeronMalik_clicked();
	void on_pushButtonGMFC_clicked();
	void on_pushButtonSelectCur_clicked();
	void on_pushButtonStopOnLast_clicked(); 
	void on_spinBoxLinDifIters_valueChanged(int value);
	void on_pushButtonDistanceFunc_clicked();
	void on_pushButtonEdgeMotion_clicked();
	void on_pushButtonGAC_clicked();
};
