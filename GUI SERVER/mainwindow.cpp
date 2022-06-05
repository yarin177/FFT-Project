#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QHostAddress>
#include <QThread>
#include <QJsonDocument>
#include <QJsonValue>
#include <QImage>
#include <QPixmap>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _socket(this)
{
    ui->setupUi(this);
    this->setFixedSize(QSize(1100, 1000));
	ui->spectro->setPixmap(QPixmap("waiting_spectro.png"));
	ui->label_11->setPixmap(QPixmap("colorbar.jpg"));
    _socket.connectToHost(QHostAddress("127.0.0.1"), 1234);
    connect(&_socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::setColorBarValues(int max_value)
{
    QLabel* entries[] = { ui->colorbar_label1, ui->colorbar_label2, ui->colorbar_label3,ui->colorbar_label4,ui->colorbar_label5,ui->colorbar_label6,ui->colorbar_label7,ui->colorbar_label8 };
    float start = 0.8;
    for(int i = 0; i < 8; i++)
    {
        int value = start * max_value;
		if(i == 0)
			entries[i]->setText(QString::number(value) + "(dBm)");
		else
			entries[i]->setText(QString::number(value));
        start -= 0.075;
    }
}
void MainWindow::setTime(int time)
{
    ui->label_10->setText(QString::number(time) + "(s)");
}

void MainWindow::onReadyRead()
{
    QByteArray datas = _socket.readAll();
    std::string message = datas.toStdString();
    //QString str = QString::fromUtf8(message.c_str());

    QJsonDocument doc = QJsonDocument::fromJson(datas);
    QString max_colorbar = doc.object().value("colorbar").toString();
    setColorBarValues(max_colorbar.toInt());
    setTime(doc.object().value("time").toString().toInt());

    QString base64img = doc.object().value("image").toString();
    QByteArray by = QByteArray::fromBase64(base64img.toLatin1());
    QImage img = QImage::fromData(by,"PNG");
    //img = img.smoothScaled(15,10);
    img = img.scaled(1000,1000);
    ui->spectro->setPixmap(QPixmap::fromImage(img));
    //qDebug() << base64_image;
}
