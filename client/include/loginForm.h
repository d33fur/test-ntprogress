#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStackedWidget>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCheckBox>
#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QComboBox>
#include <QFormLayout>
#include <QDialog>

class LoginForm : public QWidget {
    Q_OBJECT
public:
    LoginForm(QWidget *parent = nullptr);

signals:
    void loginComplete(const QString &username, const QString &token, const QString &userId);
    void loginFailed(const QString &errorMessage);

private slots:
    void loginClicked();
    void handleNetworkReply(QNetworkReply *reply);

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QCheckBox *rememberMeCheckBox;
};

#endif // LOGINFORM_H