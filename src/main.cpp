#include "ImageViewer.h"
#include <QtWidgets/QApplication>
#include <QLocale>
#include <QCoreApplication>

int main(int argc, char* argv[])
{
	QLocale::setDefault(QLocale::c());

	QCoreApplication::setOrganizationName("MPM");
	QCoreApplication::setApplicationName("ImageViewerSO");

	QApplication a(argc, argv);
	ImageViewer w;
	w.show();
	return a.exec();
}