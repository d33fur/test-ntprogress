#include "stockExchangeForm.h"

StockExchangeForm::StockExchangeForm(QWidget *parent) : QWidget(parent) {
    qDebug() << "LogFunc: StockExchangeForm()";

    nameLabel = new QLabel("Name: ");
    lastNameLabel = new QLabel("Last Name: ");
    balanceUSDLabel = new QLabel("Balance USD: ");
    balanceRUBLabel = new QLabel("Balance RUB: ");

    currentDealsButton = new QPushButton("CurrentDeals");
    myDealsButton = new QPushButton("MyDeals");
    currencyQuotesButton = new QPushButton("CurrencyQuotes");
    makeDealButton = new QPushButton("Make a Deal");
    balanceButton = new QPushButton("Update balance");
    dealsList = new QListWidget;

    dealDialog = new DealDialog(this);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *topLayout = new QHBoxLayout;
    QHBoxLayout *midLayout = new QHBoxLayout;
    QHBoxLayout *bottomLayout = new QHBoxLayout;

    bottomLayout->addWidget(balanceButton);
    bottomLayout->addWidget(nameLabel);
    bottomLayout->addWidget(lastNameLabel);
    bottomLayout->addWidget(balanceUSDLabel);
    bottomLayout->addWidget(balanceRUBLabel);

    midLayout->addWidget(dealsList);

    topLayout->addWidget(currentDealsButton);
    topLayout->addWidget(myDealsButton);
    topLayout->addWidget(currencyQuotesButton);
    topLayout->addStretch();
    topLayout->addWidget(makeDealButton);

    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(midLayout);
    mainLayout->addLayout(bottomLayout);

    setLayout(mainLayout);

    connect(currentDealsButton, &QPushButton::clicked, this, &StockExchangeForm::getCurrentDealsRequest);
    connect(myDealsButton, &QPushButton::clicked, this, &StockExchangeForm::getMyDealsRequest);
    connect(balanceButton, &QPushButton::clicked, this, &StockExchangeForm::getUserInfo);
    connect(dealDialog, &DealDialog::dealSubmitted, this, &StockExchangeForm::makeDeal);
    connect(makeDealButton, &QPushButton::clicked, this, &StockExchangeForm::showMakeDealDialog);
}

void StockExchangeForm::setUserInfo(const QString &token, const QString &id) {
    qDebug() << "LogFunc: setUserInfo()";

    userToken = token;
    userId = id;

    sendGetUserInfo(userToken, userId);
    qDebug() << userToken << " " << userId;
}

void StockExchangeForm::showMakeDealDialog() {
    qDebug() << "LogFunc: showMakeDealDialog()";

    dealDialog->show();
}

void StockExchangeForm::makeDeal(const QString &amount, const QString &exchangeRate, const QString &dealType) {
    qDebug() << "LogFunc: makeDeal()";

    bool amountIsValid = amount.toDouble() > 0;
    bool exchangeRateIsValid = exchangeRate.toDouble() > 0;

    if (!amountIsValid || !exchangeRateIsValid) {
        qDebug() << "Invalid input: Amount and Exchange Rate must be positive numbers.";
        return;
    }

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &StockExchangeForm::handleMakeDeal);

    QNetworkRequest request(QUrl("http://localhost:8001/makeDeal"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Token", userToken.toUtf8());
    request.setRawHeader("User_id", userId.toUtf8());

    QJsonObject json;
    json["deal_type"] = dealType;
    json["amount"] = amount;
    json["exchange_rate"] = exchangeRate;

    QJsonDocument jsonDoc(json);
    QByteArray data = jsonDoc.toJson();

    qDebug() << "user input data" << json;
    manager->post(request, data);
}

void StockExchangeForm::handleMakeDeal(QNetworkReply *reply) {
    qDebug() << "LogFunc: handleMakeDeal()";

    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonResponse = jsonDoc.object();
    qDebug() << jsonResponse;

    reply->deleteLater();
}

void StockExchangeForm::getCurrentDealsRequest() {
    qDebug() << "LogFunc: getCurrentDealsRequest()";

    sendGetCurrentDeals(userToken, userId);
}


void StockExchangeForm::getMyDealsRequest() {
    qDebug() << "LogFunc: getMyDealsRequest()";

    sendGetMyDeals(userToken, userId);
}

void StockExchangeForm::getUserInfo() {
    qDebug() << "LogFunc: getUserInfo()";

    sendGetUserInfo(userToken, userId);
}

void StockExchangeForm::sendGetUserInfo(const QString token, const QString userId) {
    qDebug() << "LogFunc: sendGetUserInfo()";

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &StockExchangeForm::handleGetUserInfo);

    QNetworkRequest request;
    request.setUrl(QUrl("http://localhost:8001/getMyInfo"));
    request.setRawHeader("Token", token.toUtf8());
    request.setRawHeader("User_id", userId.toUtf8());

    manager->get(request);
}

void StockExchangeForm::handleGetUserInfo(QNetworkReply *reply) {
    qDebug() << "LogFunc: handleGetUserInfo()";

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonResponse = jsonDoc.object();

        nameLabel->setText("Name: " + jsonResponse["first_name"].toString());
        lastNameLabel->setText("Last Name: " + jsonResponse["second_name"].toString());
        balanceUSDLabel->setText("BalanceUSD: " + jsonResponse["balance_usd"].toString());
        balanceRUBLabel->setText("BalanceRUB: " + jsonResponse["balance_rub"].toString());

        qDebug() << "Get request successful:" << jsonResponse;
    } else {
        qDebug() << "Get request failed:" << reply->errorString();
    }

    reply->deleteLater();
}

void StockExchangeForm::sendGetCurrentDeals(const QString token, const QString userId) {
    qDebug() << "LogFunc: sendGetCurrentDeals()";

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &StockExchangeForm::handleGetReplyCurrentDeals);

    QNetworkRequest request;
    request.setUrl(QUrl("http://localhost:8001/getCurrentDeals"));
    request.setRawHeader("Token", token.toUtf8());
    request.setRawHeader("User_id", userId.toUtf8());

    manager->get(request);
}

void StockExchangeForm::handleGetReplyCurrentDeals(QNetworkReply *reply) {
    qDebug() << "LogFunc: handleGetReplyCurrentDeals()";

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonArray jsonArray = jsonDoc.array();

        qDebug() << "Get request successful:" << jsonArray;
        displayDeals(jsonArray);
    } 
    else {
        qDebug() << "Get request failed:" << reply->errorString();
    }

    reply->deleteLater();
}

void StockExchangeForm::displayDeals(QJsonArray jsonDeals) {
    qDebug() << "LogFunc: displayDeals()";

    int size = jsonDeals.size();
    dealsList->clear();

    for (int i = 0; i < size; ++i) {
        QJsonObject deal = jsonDeals[i].toObject();

        QString amount = deal["amount"].toString();
        QString dealStatus = deal["deal_status"].toString();
        QString dealType = deal["deal_type"].toString();
        QString exchangeRate = deal["exchange_rate"].toString();
        QString fromUserId = deal["from_user_id"].toString();
        QString id = deal["id"].toString();
        QString timeStart = deal["time_start"].toString();
        QString toUserId = deal["to_user_id"].toString();

        QString dealString = QString("%1 USD "
                                    "%2 "
                                    "With exchange Rate: %3 "
                                    " From User ID: %4 "
                                    "Deal ID: %5 ")
                                .arg(amount)
                                .arg(dealType)
                                .arg(exchangeRate)
                                .arg(fromUserId)
                                .arg(id);

        dealsList->addItem(dealString);
    }
}

void StockExchangeForm::sendGetMyDeals(const QString token, const QString userId) {
    qDebug() << "LogFunc: sendGetMyDeals()";

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &StockExchangeForm::handleGetReplyMyDeals);

    QNetworkRequest request;
    request.setUrl(QUrl("http://localhost:8001/getMyDeals"));
    request.setRawHeader("Token", token.toUtf8());
    request.setRawHeader("User_id", userId.toUtf8());

    manager->get(request);
}

void StockExchangeForm::handleGetReplyMyDeals(QNetworkReply *reply) {
    qDebug() << "LogFunc: handleGetReplyMyDeals()";

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonArray jsonArray = jsonDoc.array();

        qDebug() << "Get request successful:" << jsonArray;

        displayMyDeals(jsonArray);
    } 
    else {
        qDebug() << "Get request failed:" << reply->errorString();
    }

    reply->deleteLater();
}

void StockExchangeForm::displayMyDeals(QJsonArray jsonDeals) {
    qDebug() << "LogFunc: displayMyDeals()";

    int size = jsonDeals.size();
    dealsList->clear();

    for (int i = 0; i < size; ++i) {
        QJsonObject deal = jsonDeals[i].toObject();

        QString amount = deal["amount"].toString();
        QString dealStatus = deal["deal_status"].toString();
        QString dealType = deal["deal_type"].toString();
        QString exchangeRate = deal["exchange_rate"].toString();
        QString fromUserId = deal["from_user_id"].toString();
        QString id = deal["id"].toString();
        QString timeStart = deal["time_start"].toString();
        QString toUserId = deal["to_user_id"].toString();

        QString dealString = QString("%1 USD "
                                    "%2 "
                                    "With exchange Rate: %3 "
                                    " From User ID: %4 "
                                    "Deal ID: %5 "
                                    "Status: %6 ")
                                .arg(amount)
                                .arg(dealType)
                                .arg(exchangeRate)
                                .arg(fromUserId)
                                .arg(id)
                                .arg(dealStatus);

        QWidget *dealWidget = new QWidget(dealsList);
        QVBoxLayout *layout = new QVBoxLayout(dealWidget);

        QLabel *dealLabel = new QLabel(dealString);
        layout->addWidget(dealLabel);

        if (dealStatus == "active") {
            QPushButton *cancelButton = new QPushButton("Cancel");

            connect(cancelButton, &QPushButton::clicked, this, [this, id]() {
                handleCancelDeal(id);
            });

            layout->addWidget(cancelButton);
        }

        dealWidget->setLayout(layout);
        QListWidgetItem *item = new QListWidgetItem(dealsList);
        item->setSizeHint(dealWidget->sizeHint());
        dealsList->setItemWidget(item, dealWidget);
    }
}

void StockExchangeForm::handleCancelDeal(const QString &dealId) {
    qDebug() << "LogFunc: handleCancelDeal()";

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &StockExchangeForm::handleCancelDealReply);

    QNetworkRequest request(QUrl("http://localhost:8001/cancelDeal"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Token", userToken.toUtf8());
    request.setRawHeader("User_id", userId.toUtf8());

    QJsonObject json;
    json["deal_id"] = dealId;

    QJsonDocument jsonDoc(json);
    QByteArray data = jsonDoc.toJson();

    qDebug() << "Cancel deal data:" << json;
    manager->post(request, data);
}

void StockExchangeForm::handleCancelDealReply(QNetworkReply *reply) {
    qDebug() << "LogFunc: handleCancelDealReply()";

    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonResponse = jsonDoc.object();
    qDebug() << jsonResponse;

    reply->deleteLater();
}

// #include "stockExchangeForm.moc"