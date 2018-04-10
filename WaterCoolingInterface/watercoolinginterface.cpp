#include "watercoolinginterface.h"

WaterCoolingInterface::WaterCoolingInterface(QWidget *parent)
	: QMainWindow(parent)
{
	time_ = 0;
	frequence_ = 1;

	tcp_connect_ = "1";
	tcp_disconnect_ = "0";

	host_address_ = QHostAddress("192.168.4.1");
	port_ = 8080;

	ui.setupUi(this);
	ui.pushButton_close->setEnabled(false);

	// 设置能显示的位数
	ui.temperature->setDigitCount(5);
	// 设置显示的模式为十进制
	ui.temperature->setMode(QLCDNumber::Dec);
	// 设置显示外观
	ui.temperature->setSegmentStyle(QLCDNumber::Flat);
	// 设置样式
	ui.temperature->setStyleSheet("color: rgb(180,252,253); background: black");

	pa_.setColor(QPalette::WindowText, Qt::red);
	ui.label_connected->setPalette(pa_);
	QString not_connected = QStringLiteral("未连接");
	ui.label_connected->setText(not_connected);
	QFont font("Microsoft YaHei", 12, 75);
	ui.label_connected->setFont(font);

	connect(ui.pushButton_open, SIGNAL(clicked()), this, SLOT(TcpConnect()));
	connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(TcpDisconnect()));
	connect(ui.pushButton_set_parameter, SIGNAL(clicked()), this, SLOT(SetParameter()));
	connect(&tcp_client_, SIGNAL(connected()), this, SLOT(TcpConnected()));
	connect(&tcp_client_, SIGNAL(readyRead()), this, SLOT(TcpReceive()));
	connect(&tcp_client_, SIGNAL(disconnected()), this, SLOT(TcpDisconnected()));
}

WaterCoolingInterface::~WaterCoolingInterface()
{
}

void WaterCoolingInterface::TcpConnect()
{
	pa_.setColor(QPalette::WindowText, Qt::blue);
	ui.label_connected->setPalette(pa_);
	ui.pushButton_open->setEnabled(false);
	ui.pushButton_close->setEnabled(true);
	QString is_connecting = QStringLiteral("连接中");
	ui.label_connected->setText(is_connecting);
	tcp_client_.connectToHost(host_address_, port_);
}

void WaterCoolingInterface::TcpConnected() {
	pa_.setColor(QPalette::WindowText, Qt::green);
	ui.label_connected->setPalette(pa_);
	QString has_connected = QStringLiteral("已连接");
	ui.label_connected->setText(has_connected);
	tcp_client_.write(tcp_connect_);
}

void WaterCoolingInterface::TcpReceive() {
	received_data_ = tcp_client_.readAll();
	Plot(received_data_.toDouble());
}

void WaterCoolingInterface::TcpDisconnect() {
	if (ui.label_connected->text() != QStringLiteral("已连接"))
	{
		this->TcpDisconnected();
	}
	else
	{
		tcp_client_.write(tcp_disconnect_);
		tcp_client_.disconnectFromHost();
	}
}

void WaterCoolingInterface::TcpDisconnected()
{
	if (ui.label_connected->text() == QStringLiteral("已连接"))
	{
		exit(0);
	}

	pa_.setColor(QPalette::WindowText, Qt::red);
	ui.label_connected->setPalette(pa_);
	ui.pushButton_close->setEnabled(false);
	QString has_disconnected = QStringLiteral("已断开");
	ui.label_connected->setText(has_disconnected);
	ui.pushButton_open->setEnabled(true);
}

void WaterCoolingInterface::PlotInitialize()
{
	//设置四边都有坐标
	ui.graph->axisRect()->setupFullAxesBox(true);

	//设置坐标颜色/坐标名称颜色
	ui.graph->yAxis->setLabelColor(TextColor);
	ui.graph->xAxis->setLabelColor(TextColor);
	ui.graph->xAxis->setTickLabelColor(TextColor);
	ui.graph->yAxis->setTickLabelColor(TextColor);
	ui.graph->xAxis->setBasePen(QPen(TextColor, TextWidth));
	ui.graph->yAxis->setBasePen(QPen(TextColor, TextWidth));
	ui.graph->xAxis->setTickPen(QPen(TextColor, TextWidth));
	ui.graph->yAxis->setTickPen(QPen(TextColor, TextWidth));
	ui.graph->xAxis->setSubTickPen(QPen(TextColor, TextWidth));
	ui.graph->yAxis->setSubTickPen(QPen(TextColor, TextWidth));
	ui.graph->yAxis2->setLabelColor(TextColor);
	ui.graph->xAxis2->setLabelColor(TextColor);
	ui.graph->xAxis2->setTickLabelColor(TextColor);
	ui.graph->yAxis2->setTickLabelColor(TextColor);
	ui.graph->xAxis2->setBasePen(QPen(TextColor, TextWidth));
	ui.graph->yAxis2->setBasePen(QPen(TextColor, TextWidth));
	ui.graph->xAxis2->setTickPen(QPen(TextColor, TextWidth));
	ui.graph->yAxis2->setTickPen(QPen(TextColor, TextWidth));
	ui.graph->xAxis2->setSubTickPen(QPen(TextColor, TextWidth));
	ui.graph->yAxis2->setSubTickPen(QPen(TextColor, TextWidth));

	//设置画布背景色
	QLinearGradient plotGradient;
	plotGradient.setStart(0, 0);
	plotGradient.setFinalStop(0, 350);
	plotGradient.setColorAt(0, QColor(80, 80, 80));
	plotGradient.setColorAt(1, QColor(50, 50, 50));
	ui.graph->setBackground(plotGradient);

	//设置坐标背景色
	QLinearGradient axisRectGradient;
	axisRectGradient.setStart(0, 0);
	axisRectGradient.setFinalStop(0, 350);
	axisRectGradient.setColorAt(0, QColor(80, 80, 80));
	axisRectGradient.setColorAt(1, QColor(30, 30, 30));
	ui.graph->axisRect()->setBackground(axisRectGradient);

	//设置图例提示位置及背景色
	ui.graph->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
	ui.graph->legend->setBrush(QColor(255, 255, 255, 200));

	//添加曲线
	ui.graph->addGraph();
	ui.graph->addGraph();
	ui.graph->graph(0)->setPen(QPen(Plot_DotColor, DotWidth));
	ui.graph->graph(1)->setPen(QPen(Plot_LineColor, LineWidth));

	//设置坐标轴
	QString time_string = QStringLiteral("时间/秒");
	ui.graph->xAxis->setLabel(time_string);
	QString temperature_string = QStringLiteral("温度/摄氏度");
	ui.graph->yAxis->setLabel(temperature_string);

	ui.graph->xAxis->setRange(0, 100);
	ui.graph->yAxis->setRange(0, 70);

	ui.graph->replot();
}

void WaterCoolingInterface::Plot(double data)
{
	if (time_ == ui.graph->xAxis->range().upper) {
		int gap = ui.graph->xAxis->range().upper - ui.graph->xAxis->range().lower;
		ui.graph->xAxis->setRange(ui.graph->xAxis->range().lower + gap, ui.graph->xAxis->range().upper + gap);
	}

	if (!ui.graph->graph(1)->data()->isEmpty())
	{
		ui.graph->graph(1)->data()->clear();
	}

	ui.graph->graph(1)->addData(time_, ui.graph->yAxis->range().lower);
	ui.graph->graph(1)->addData(time_, ui.graph->yAxis->range().upper);
	ui.temperature->display(data);
	ui.graph->graph(0)->addData(time_, data);
	
	ui.graph->replot();
	time_ += 1/frequence_;
}

void WaterCoolingInterface::SetParameter() {
	QString string_host_address, string_port, string_x_axis_start_point, string_x_axis_end_point, string_y_axis_start_point, string_y_axis_end_point;
	string_host_address = ui.plainTextEdit_host_address->toPlainText();
	string_port = ui.plainTextEdit_port->toPlainText();
	string_x_axis_start_point = ui.plainTextEdit_x_axis_start_point->toPlainText();
	string_x_axis_end_point = ui.plainTextEdit_x_axis_end_point->toPlainText();
	string_y_axis_start_point = ui.plainTextEdit_y_axis_start_point->toPlainText();
	string_y_axis_end_point = ui.plainTextEdit_y_axis_end_point->toPlainText();

	if (string_host_address.length() != 0) {
		host_address_ = QHostAddress(string_host_address);
	}

	if (string_port.length() != 0) {
		port_ = string_port.toInt();
	}

	if (string_x_axis_start_point.length() != 0) {
		ui.graph->xAxis->setRange(string_x_axis_start_point.toInt(), ui.graph->xAxis->range().upper);
	}

	if (string_x_axis_end_point.length() != 0) {
		ui.graph->xAxis->setRange(ui.graph->xAxis->range().lower, string_x_axis_end_point.toInt());
	}

	if (string_y_axis_start_point.length() != 0) {
		ui.graph->yAxis->setRange(string_y_axis_start_point.toInt(), ui.graph->yAxis->range().upper);
	}

	if (string_y_axis_end_point.length() != 0) {
		ui.graph->yAxis->setRange(ui.graph->yAxis->range().lower, string_y_axis_end_point.toInt());
	}

	ui.graph->replot();
}