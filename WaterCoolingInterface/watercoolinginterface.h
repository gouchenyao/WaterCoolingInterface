#ifndef WATERCOOLINGINTERFACE_H
#define WATERCOOLINGINTERFACE_H

#include "ui_watercoolinginterface.h"

#include <QtWidgets/QMainWindow>
#include <QtNetwork>

#include "qcustomplot.h"

#define TextColor QColor(255,255,255)
#define Plot_NoColor QColor(0,0,0,0)

#define Plot_DotColor QColor(255,255,255)
#define Plot_LineColor QColor(255,0,0)

#define TextWidth 1
#define DotWidth 2
#define LineWidth 3

class WaterCoolingInterface : public QMainWindow
{
	Q_OBJECT

public:
	WaterCoolingInterface(QWidget *parent = 0);
	~WaterCoolingInterface();
	void PlotInitialize();
	void Plot(double data);

private:
	Ui::WaterCoolingInterfaceClass ui;
	QTcpSocket tcp_client_;
	char* tcp_connect_;
	char* tcp_disconnect_;
	QByteArray received_data_;
	double time_, frequence_;
	QPalette pa_;
	QHostAddress host_address_;
	int port_;

public slots:
	void TcpConnect();
	void TcpConnected();
	void TcpReceive();
	void TcpDisconnect();
	void TcpDisconnected();
	void SetParameter();
};

#endif // WATERCOOLINGINTERFACE_H
