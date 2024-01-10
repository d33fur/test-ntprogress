#ifndef REGISTRATIONFORM_H
#define REGISTRATIONFORM_H

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

class RegistrationForm : public QWidget {
    Q_OBJECT
public:
    RegistrationForm(QWidget *parent = nullptr);

signals:
    void registrationComplete(const QString &username, const QString &token, const QString &userId);
    void registrationFailed(const QString &errorMessage);

private slots:
    void registerClicked();
    void handleNetworkReply(QNetworkReply *reply);

private:
    QLineEdit *firstNameEdit;
    QLineEdit *lastNameEdit;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QCheckBox *rememberMeCheckBox;
};

#endif // REGISTRATIONFORM_H