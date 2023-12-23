#include "client.h"
#include <QGridLayout>
#include <QtNetwork>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QScrollArea>
#include <iostream>
#include <QPalette>
#include <QFileDialog>
#include <QPixmap>
#include <QPainter>
#include <fstream>

Login::Login(QWidget *parent) : QWidget(parent) {

}