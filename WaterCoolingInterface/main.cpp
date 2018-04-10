#include "watercoolinginterface.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	WaterCoolingInterface w;
	w.PlotInitialize();
	w.show();
	return a.exec();
}