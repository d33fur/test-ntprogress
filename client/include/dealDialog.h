#ifndef DEALDIALOG_H
#define DEALDIALOG_H

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

class DealDialog : public QDialog {
    Q_OBJECT

public:
    DealDialog(QWidget *parent = nullptr);

signals:
    void dealSubmitted(const QString &amount, const QString &exchangeRate, const QString &dealType);

private slots:
    void handleSubmit();

private:
    QLineEdit *amountLineEdit;
    QLineEdit *exchangeRateLineEdit;
    QComboBox *dealTypeComboBox;
    QPushButton *submitButton;
    QFormLayout *layout;
};

#endif // DEALDIALOG_H