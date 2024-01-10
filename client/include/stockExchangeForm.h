#ifndef STOCKEXCHANGEFORM_H
#define STOCKEXCHANGEFORM_H

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
#include "dealDialog.h"

class StockExchangeForm : public QWidget {
    Q_OBJECT
public:
    StockExchangeForm(QWidget *parent = nullptr);

signals:
    void getCurrentDeals(const QString token, const QString userId);
    
public slots:
    void setUserInfo(const QString &token, const QString &id);

private slots:
    void showMakeDealDialog();
    void makeDeal(const QString &amount, const QString &exchangeRate, const QString &dealType);
    void handleMakeDeal(QNetworkReply *reply);
    void getCurrentDealsRequest();
    void getMyDealsRequest();
    void getUserInfo();
    void sendGetUserInfo(const QString token, const QString userId);
    void handleGetUserInfo(QNetworkReply *reply);
    void sendGetCurrentDeals(const QString token, const QString userId);
    void handleGetReplyCurrentDeals(QNetworkReply *reply);
    void displayDeals(QJsonArray jsonDeals);
    void sendGetMyDeals(const QString token, const QString userId);
    void handleGetReplyMyDeals(QNetworkReply *reply);
    void displayMyDeals(QJsonArray jsonDeals);
    void handleCancelDeal(const QString &dealId);
    void handleCancelDealReply(QNetworkReply *reply);

private:
    QLabel *nameLabel;
    QLabel *lastNameLabel;
    QLabel *balanceUSDLabel;
    QLabel *balanceRUBLabel;
    QListWidget *dealsList;
    DealDialog *dealDialog;

    QPushButton *currentDealsButton;
    QPushButton *myDealsButton;
    QPushButton *currencyQuotesButton;
    QPushButton *makeDealButton;
    QPushButton *balanceButton;

    QString userToken;
    QString userId;
};

#endif // STOCKEXCHANGEFORM_H