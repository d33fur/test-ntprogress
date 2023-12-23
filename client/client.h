#ifndef CLIENT_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QtNetwork/QNetworkAccessManager>
#include <QSlider>
#include <vector>
#include <QJsonArray>
#include <QJsonObject>
#include <QSignalMapper>
#include <QApplication>
#include <QComboBox>
#include <QCheckBox>
#include <QScrollArea>
#include <QGridLayout>
class Login : public QWidget {
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);

private:

};
#endif // FRONTEND_H