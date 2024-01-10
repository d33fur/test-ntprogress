#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void showRegistrationForm();
    void showLoginForm();
    void showAuthForm();
    void handleRegistrationFailure(const QString &errorMessage);
    void handleLoginFailure(const QString &errorMessage);
    void handleLoginComplete(const QString &username, const QString &token, const QString &userId);

signals:
    void userAuthenticated(const QString &token, const QString &userId);

private:
    QPushButton *signupButton;
    QPushButton *loginButton;
    QPushButton *logoutButton;
    QStackedWidget *stackedWidget;
};

#endif // MAINWINDOW_H