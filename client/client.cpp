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
    DealDialog(QWidget *parent = nullptr) : QDialog(parent) {
        qDebug() << "LogFunc: DealDialog()";
        setWindowTitle("Make Deal");

        // Создаем элементы управления
        amountLineEdit = new QLineEdit(this);
        exchangeRateLineEdit = new QLineEdit(this);
        dealTypeComboBox = new QComboBox(this);
        dealTypeComboBox->addItem("buy");
        dealTypeComboBox->addItem("sell");

        QPushButton *submitButton = new QPushButton("Submit", this);

        // Создаем layout и добавляем элементы
        QFormLayout *layout = new QFormLayout(this);
        layout->addRow("Amount:", amountLineEdit);
        layout->addRow("Exchange Rate:", exchangeRateLineEdit);
        layout->addRow("Deal Type:", dealTypeComboBox);
        layout->addWidget(submitButton);

        // Подключаем сигнал нажатия кнопки к обработчику
        connect(submitButton, &QPushButton::clicked, this, &DealDialog::handleSubmit);
        // Установка фиксированного размера окна
        setFixedSize(300, 150);
    }

signals:
    void dealSubmitted(const QString &amount, const QString &exchangeRate, const QString &dealType);
    void dealMade();

private slots:
    void handleSubmit() {
        qDebug() << "LogFunc: handleSubmit()";
        // Отправляем сигнал с данными введенными пользователем

        emit dealSubmitted(amountLineEdit->text(), exchangeRateLineEdit->text(), dealTypeComboBox->currentText());
        emit dealMade();
        // Закрываем окно
        close();
    }

private:
    QLineEdit *amountLineEdit;
    QLineEdit *exchangeRateLineEdit;
    QComboBox *dealTypeComboBox;
};

class RegistrationForm : public QWidget {
    Q_OBJECT
public:
    RegistrationForm(QWidget *parent = nullptr) : QWidget(parent) {
        qDebug() << "LogFunc: RegistrationForm()";
        QLabel *firstNameLabel = new QLabel("First Name:");
        QLabel *lastNameLabel = new QLabel("Last Name:");
        QLabel *usernameLabel = new QLabel("Username:");
        QLabel *passwordLabel = new QLabel("Password:");
        QLabel *rememberMeLabel = new QLabel("Remember Me?");

        firstNameEdit = new QLineEdit;
        lastNameEdit = new QLineEdit;
        usernameEdit = new QLineEdit;
        passwordEdit = new QLineEdit;
        rememberMeCheckBox = new QCheckBox;

        QPushButton *registerButton = new QPushButton("Register");

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(firstNameLabel);
        layout->addWidget(firstNameEdit);
        layout->addWidget(lastNameLabel);
        layout->addWidget(lastNameEdit);
        layout->addWidget(usernameLabel);
        layout->addWidget(usernameEdit);
        layout->addWidget(passwordLabel);
        layout->addWidget(passwordEdit);
        layout->addWidget(rememberMeLabel);
        layout->addWidget(rememberMeCheckBox);
        layout->addWidget(registerButton);

        setLayout(layout);

        connect(registerButton, &QPushButton::clicked, this, &RegistrationForm::registerClicked);
    }

signals:
    void registrationComplete(const QString &username, const QString &token, const QString &userId);
    void registrationFailed(const QString &errorMessage);
private slots:
    void registerClicked() {
        qDebug() << "LogFunc: registerClicked()";
        QString firstName = firstNameEdit->text();
        QString lastName = lastNameEdit->text();
        QString username = usernameEdit->text();
        QString password = passwordEdit->text();
        // QString confirmPassword = confirmPasswordEdit->text();
        bool rememberMe = rememberMeCheckBox->isChecked();

        // if (password != confirmPassword) {
        //     // Обработка ошибки - пароли не совпадают
        //     return;
        // }


        QJsonObject json;
        json["first_name"] = firstName;
        json["second_name"] = lastName;
        json["login"] = username;
        json["password"] = password;
        json["remember_me"] = rememberMe ? "1" : "0";

        QJsonDocument jsonDoc(json);
        QByteArray data = jsonDoc.toJson();
        qDebug() << "user input data" << json;

        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, &QNetworkAccessManager::finished, this, &RegistrationForm::handleNetworkReply);

        QNetworkRequest request(QUrl("http://localhost:8001/signup"));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        manager->post(request, data);

        QString token = "";
        QString userId = "";
    }

    void handleNetworkReply(QNetworkReply *reply) {
        qDebug() << "LogFunc: handleNetworkReply()";
        if (reply->error() == QNetworkReply::NoError) {

            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            QJsonObject jsonResponse = jsonDoc.object();

            if (jsonResponse.contains("token") && jsonResponse.contains("user_id")) {
                QString token = jsonResponse["token"].toString();
                QString userId = jsonResponse["user_id"].toString();
                emit registrationComplete(usernameEdit->text(), token, userId);
            } else if (jsonResponse.contains("User already exists")){
                emit registrationFailed("User already exists");
            } else {
                emit registrationFailed("something goes wrong with backend");
            }
        } else {
            emit registrationFailed(reply->errorString());
        }

        reply->deleteLater();
    }

private:
    QLineEdit *firstNameEdit;
    QLineEdit *lastNameEdit;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    // QLineEdit *confirmPasswordEdit;
    QCheckBox *rememberMeCheckBox;
};

class LoginForm : public QWidget {
    Q_OBJECT
public:
    LoginForm(QWidget *parent = nullptr) : QWidget(parent) {
        qDebug() << "LogFunc: LoginForm()";
        QLabel *usernameLabel = new QLabel("Username:");
        QLabel *passwordLabel = new QLabel("Password:");

        usernameEdit = new QLineEdit;
        passwordEdit = new QLineEdit;

        rememberMeCheckBox = new QCheckBox("Remember Me");

        QPushButton *loginButton = new QPushButton("Login");

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(usernameLabel);
        layout->addWidget(usernameEdit);
        layout->addWidget(passwordLabel);
        layout->addWidget(passwordEdit);
        layout->addWidget(rememberMeCheckBox);
        layout->addWidget(loginButton);

        setLayout(layout);

        connect(loginButton, &QPushButton::clicked, this, &LoginForm::loginClicked);
    }

signals:
    void loginComplete(const QString &username, const QString &token, const QString &userId);
    void loginFailed(const QString &errorMessage);

private slots:
    void loginClicked() {
        qDebug() << "LogFunc: loginClicked()";
        QString username = usernameEdit->text();
        QString password = passwordEdit->text();


        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, &QNetworkAccessManager::finished, this, &LoginForm::handleNetworkReply);

        QNetworkRequest request(QUrl("http://localhost:8001/login"));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");


        QJsonObject json;
        json["login"] = username;
        json["password"] = password;

        QJsonDocument jsonDoc(json);
        QByteArray data = jsonDoc.toJson();
        qDebug() << "user input data" << json;
        manager->post(request, data);

        QString token = "";
        QString userId = "";
    }

    void handleNetworkReply(QNetworkReply *reply) {
        qDebug() << "LogFunc: handleNetworkReply()";
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            QJsonObject jsonResponse = jsonDoc.object();

            if (jsonResponse.contains("token") && jsonResponse.contains("user_id")) {
                QString token = jsonResponse["token"].toString();
                QString userId = jsonResponse["user_id"].toString();
                emit loginComplete(usernameEdit->text(), token, userId);
            } else if (jsonResponse.contains("Wrong login or password")) {
                emit loginFailed("Wrong login or password");
            } else {
                emit loginFailed("something goes wrong");
            }
        } else {
            emit loginFailed(reply->errorString());
        }

        reply->deleteLater();
        
    }

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QCheckBox *rememberMeCheckBox;
};


class StockExchangeForm : public QWidget {
    Q_OBJECT
public:
    StockExchangeForm(QWidget *parent = nullptr) : QWidget(parent) {
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
        //connect(makeDealButton, &QPushButton::clicked, this, &StockExchangeForm::makeDealShow);

        dealDialog = new DealDialog(this);

        connect(dealDialog, &DealDialog::dealSubmitted, this, &StockExchangeForm::makeDeal);
        connect(makeDealButton, &QPushButton::clicked, this, &StockExchangeForm::showMakeDealDialog);
        connect(dealDialog, &DealDialog::dealMade, this, &StockExchangeForm::getMyDealsRequest);
    }

signals:
    void getCurrentDeals(const QString token, const QString userId);
    

public slots:
    void setUserInfo(const QString &token, const QString &id) {
        qDebug() << "LogFunc: setUserInfo()";

        userToken = token;
        userId = id;
        sendGetUserInfo(userToken, userId);
        //nameLabel->setText()
        // userToken = token;
        // userId = id;
        qDebug() << userToken << " " << userId;
    }

private slots:

    void showMakeDealDialog() {
        qDebug() << "LogFunc: showMakeDealDialog()";
        dealDialog->show();
    }


    void makeDeal(const QString &amount, const QString &exchangeRate, const QString &dealType) {
        qDebug() << "LogFunc: makeDeal()";
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

    void handleMakeDeal(QNetworkReply *reply) {
        qDebug() << "LogFunc: handleMakeDeal()";

        reply->deleteLater();
        
    }

    void getCurrentDealsRequest() {
        qDebug() << "LogFunc: getCurrentDealsRequest()";

        sendGetCurrentDeals(userToken, userId);
    }

    
    void getMyDealsRequest() {
        qDebug() << "LogFunc: getMyDealsRequest()";

        sendGetMyDeals(userToken, userId);
    }

    void getUserInfo() {
        qDebug() << "LogFunc: getUserInfo()";
        sendGetUserInfo(userToken, userId);
    }

    void sendGetUserInfo(const QString token, const QString userId) {
        qDebug() << "LogFunc: sendGetUserInfo()";
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, &QNetworkAccessManager::finished, this, &StockExchangeForm::handleGetUserInfo);

        QNetworkRequest request;
        request.setUrl(QUrl("http://localhost:8001/getMyInfo"));
        request.setRawHeader("Token", token.toUtf8());
        request.setRawHeader("User_id", userId.toUtf8());

        manager->get(request);
        // sendGetRequest("http://localhost:8001/getMyInfo", token, userId);
        // sendGetRequest("http://localhost:8001/getCurrencyQuotes", token, userId);
    }

    void handleGetUserInfo(QNetworkReply *reply) {
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

    void sendGetCurrentDeals(const QString token, const QString userId) {
        qDebug() << "LogFunc: sendGetCurrentDeals()";
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, &QNetworkAccessManager::finished, this, &StockExchangeForm::handleGetReplyCurrentDeals);

        QNetworkRequest request;
        request.setUrl(QUrl("http://localhost:8001/getCurrentDeals"));
        request.setRawHeader("Token", token.toUtf8());
        request.setRawHeader("User_id", userId.toUtf8());

        manager->get(request);
        // sendGetRequest("http://localhost:8001/getMyDeals", token, userId);
        // sendGetRequest("http://localhost:8001/getMyInfo", token, userId);
        // sendGetRequest("http://localhost:8001/getCurrencyQuotes", token, userId);
    }

    void handleGetReplyCurrentDeals(QNetworkReply *reply) {
        qDebug() << "LogFunc: handleGetReplyCurrentDeals()";
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            QJsonArray jsonArray = jsonDoc.array();

            qDebug() << "Get request successful:" << jsonArray;
            displayDeals(jsonArray);
        } else {
            qDebug() << "Get request failed:" << reply->errorString();
        }

        reply->deleteLater();
    }

    void displayDeals(QJsonArray jsonDeals) {
        qDebug() << "LogFunc: displayDeals()";

        int size = jsonDeals.size();
        dealsList->clear();  // Clear existing items

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

            // QString dealString = QString("Amount: %1\n"
            //                             "Deal Status: %2\n"
            //                             "Deal Type: %3\n"
            //                             "Exchange Rate: %4\n"
            //                             "From User ID: %5\n"
            //                             "ID: %6\n"
            //                             "Time Start: %7\n"
            //                             "To User ID: %8\n")
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

            qDebug() << "Deal:" << dealString;
            dealsList->addItem(dealString);
        }
    }


    void sendGetMyDeals(const QString token, const QString userId) {
        qDebug() << "LogFunc: sendGetMyDeals()";
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, &QNetworkAccessManager::finished, this, &StockExchangeForm::handleGetReplyMyDeals);

        QNetworkRequest request;
        request.setUrl(QUrl("http://localhost:8001/getMyDeals"));
        request.setRawHeader("Token", token.toUtf8());
        request.setRawHeader("User_id", userId.toUtf8());

        manager->get(request);
        // sendGetRequest("http://localhost:8001/getMyDeals", token, userId);
        // sendGetRequest("http://localhost:8001/getMyInfo", token, userId);
        // sendGetRequest("http://localhost:8001/getCurrencyQuotes", token, userId);
    }

    void handleGetReplyMyDeals(QNetworkReply *reply) {
        qDebug() << "LogFunc: handleGetReplyMyDeals()";
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            QJsonArray jsonArray = jsonDoc.array();

            qDebug() << "Get request successful:" << jsonArray;
            displayMyDeals(jsonArray);
        } else {
            qDebug() << "Get request failed:" << reply->errorString();
        }

        reply->deleteLater();
    }

    void displayMyDeals(QJsonArray jsonDeals) {
        qDebug() << "LogFunc: displayMyDeals()";

        int size = jsonDeals.size();
        dealsList->clear();  // Clear existing items

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

            // QString dealString = QString("Amount: %1\n"
            //                             "Deal Status: %2\n"
            //                             "Deal Type: %3\n"
            //                             "Exchange Rate: %4\n"
            //                             "From User ID: %5\n"
            //                             "ID: %6\n"
            //                             "Time Start: %7\n"
            //                             "To User ID: %8\n")
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

            qDebug() << "Deal:" << dealString;
            // dealsList->addItem(dealString);
            QWidget *dealWidget = new QWidget(dealsList);
            QVBoxLayout *layout = new QVBoxLayout(dealWidget);

            QLabel *dealLabel = new QLabel(dealString);
            layout->addWidget(dealLabel);

            // Conditionally add the "Cancel" button if dealStatus is "active"
            if (dealStatus == "active") {
                QPushButton *cancelButton = new QPushButton("Cancel");

                // Connect the "Cancel" button to the handleCancelDeal slot
                connect(cancelButton, &QPushButton::clicked, this, [this, id]() {
                    handleCancelDeal(id);
                });

                // Add the "Cancel" button to the layout
                layout->addWidget(cancelButton);
            }

            // Set the layout for the deal widget
            dealWidget->setLayout(layout);

            // Add the deal widget to the list
            QListWidgetItem *item = new QListWidgetItem(dealsList);
            item->setSizeHint(dealWidget->sizeHint());  // Set the size hint
            dealsList->setItemWidget(item, dealWidget);
        }
    }

    
    void handleCancelDeal(const QString &dealId) {
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

    void handleCancelDealReply(QNetworkReply *reply) {
        qDebug() << "LogFunc: handleCancelDealReply()";
        sendGetMyDeals(userToken, userId);
        // Handle the reply, e.g., emit a signal or update the UI
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "Cancel deal successful";
            
        } else {
            qDebug() << "Cancel deal failed:" << reply->errorString();
        }

        reply->deleteLater();
    }



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


class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr) : QWidget(parent) {
        qDebug() << "LogFunc: MainWindow()";
        resize(800, 600);

        signupButton = new QPushButton("Signup");
        loginButton = new QPushButton("Login");
        logoutButton = new QPushButton("Logout");

        stackedWidget = new QStackedWidget;

        RegistrationForm *registrationForm = new RegistrationForm;
        LoginForm *loginForm = new LoginForm;
        StockExchangeForm *stockExchangeForm = new StockExchangeForm;


        stackedWidget->addWidget(registrationForm);
        stackedWidget->addWidget(loginForm);
        stackedWidget->addWidget(stockExchangeForm);

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(signupButton);
        layout->addWidget(loginButton);
        layout->addWidget(stackedWidget);
        layout->addWidget(logoutButton);
        logoutButton->setVisible(false);
        stackedWidget->setVisible(false);
        setLayout(layout);

        connect(signupButton, &QPushButton::clicked, this, &MainWindow::showRegistrationForm);
        connect(loginButton, &QPushButton::clicked, this, &MainWindow::showLoginForm);
        connect(logoutButton, &QPushButton::clicked, this, &MainWindow::showAuthFrom);

        connect(registrationForm, &RegistrationForm::registrationComplete, this, &MainWindow::handleLoginComplete);
        connect(registrationForm, &RegistrationForm::registrationFailed, this, &MainWindow::handleRegistrationFailure);

        connect(loginForm, &LoginForm::loginComplete, this, &MainWindow::handleLoginComplete);
        connect(loginForm, &LoginForm::loginFailed, this, &MainWindow::handleLoginFailure);

        
        
        //connect(this, &MainWindow::userAuthenticated, this, &MainWindow::sendGetCurrentDeals);
        
        // connect(this, &MainWindow::userAuthenticated, this, &MainWindow::sendGetRequests);
        // connect(this, &MainWindow::userAuthenticated, this, &MainWindow::sendGetRequests);
        // connect(this, &MainWindow::userAuthenticated, this, &MainWindow::sendGetRequests);
        connect(this, &MainWindow::userAuthenticated, stockExchangeForm, &StockExchangeForm::setUserInfo);
    }

private slots:



    void showRegistrationForm() {
        qDebug() << "LogFunc: showRegistrationForm()";
        // stockExchangeForm->setVisible(true);
        stackedWidget->setVisible(true);
        stackedWidget->setCurrentIndex(0);  // Индекс формы регистрации
    }

    void showLoginForm() {
        qDebug() << "LogFunc: showLoginForm()";
        // stockExchangeForm->setVisible(true);
        stackedWidget->setVisible(true);
        stackedWidget->setCurrentIndex(1);  // Индекс формы входа
    }

    void showAuthFrom() {
        qDebug() << "LogFunc: showAuthFrom()";
        stackedWidget->setVisible(false);
        signupButton->setVisible(true);
        loginButton->setVisible(true);
        logoutButton->setVisible(false);
        stackedWidget->setCurrentIndex(0);  // Индекс формы входа
    }

    void handleRegistrationFailure(const QString &errorMessage) {
        qDebug() << "LogFunc: handleRegistrationFailure()";
        qDebug() << "Registration failed:" << errorMessage;
    }

    void handleLoginFailure(const QString &errorMessage) {
        qDebug() << "LogFunc: handleLoginFailure()";
        qDebug() << "Login failed:" << errorMessage;
    }

    void handleLoginComplete(const QString &username, const QString &token, const QString &userId) {
        qDebug() << "LogFunc: handleLoginComplete()";

        qDebug() << "Login complete for user:" << username;
        qDebug() << "Token:" << token;
        qDebug() << "User ID:" << userId;



        signupButton->setVisible(false);
        loginButton->setVisible(false);
        logoutButton->setVisible(true);
        stackedWidget->setCurrentIndex(2);  // Индекс главной страницы
        emit userAuthenticated(token, userId);
    }

    



signals:
    void userAuthenticated(const QString &token, const QString &userId);

private:
    QPushButton *signupButton;
    QPushButton *loginButton;
    QPushButton *logoutButton;
    QStackedWidget *stackedWidget;


};


int main(int argc, char **argv) {
    qDebug() << "LogFunc: main()";
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}

#include "client.moc"
