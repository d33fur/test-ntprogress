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

        QVBoxLayout *mainLayout = new QVBoxLayout;
        QHBoxLayout *topLayout = new QHBoxLayout;
        QHBoxLayout *bottomLayout = new QHBoxLayout;

        bottomLayout->addWidget(nameLabel);
        bottomLayout->addWidget(lastNameLabel);
        bottomLayout->addWidget(balanceUSDLabel);
        bottomLayout->addWidget(balanceRUBLabel);

        topLayout->addWidget(currentDealsButton);
        topLayout->addWidget(myDealsButton);
        topLayout->addWidget(currencyQuotesButton);
        topLayout->addStretch();
        topLayout->addWidget(makeDealButton);

        mainLayout->addLayout(topLayout);
        mainLayout->addLayout(bottomLayout);

        setLayout(mainLayout);
    }

signals:


private slots:
    void updateUserInfo(const QString &name, const QString &lastName, const QString &balanceUSD, const QString &balanceRUB) {
        nameLabel->setText("Name: " + name);
        lastNameLabel->setText("Last Name: " + lastName);
        balanceUSDLabel->setText("Balance: " + balanceUSD);
        balanceRUBLabel->setText("Balance: " + balanceRUB);
    }


private:
    QLabel *nameLabel;
    QLabel *lastNameLabel;
    QLabel *balanceUSDLabel;
    QLabel *balanceRUBLabel;

    QPushButton *currentDealsButton;
    QPushButton *myDealsButton;
    QPushButton *currencyQuotesButton;
    QPushButton *makeDealButton;
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

        connect(this, &MainWindow::userAuthenticated, this, &MainWindow::sendGetCurrentDeals);
        // connect(this, &MainWindow::userAuthenticated, this, &MainWindow::sendGetRequests);
        // connect(this, &MainWindow::userAuthenticated, this, &MainWindow::sendGetRequests);
        // connect(this, &MainWindow::userAuthenticated, this, &MainWindow::sendGetRequests);
        // connect(this, &MainWindow::userAuthenticated, this, &MainWindow::sendGetRequests);
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
        qDebug() << "LogFunc: logout()";
        stackedWidget->setVisible(false);
        signupButton->setVisible(true);
        loginButton->setVisible(true);
        logoutButton->setVisible(false);
        stackedWidget->setCurrentIndex(0);  // Индекс формы входа
    }

    // void handleRegistrationComplete(const QString &username, const QString &token, const QString &userId) {
    //     qDebug() << "LogFunc: handleRegistrationComplete()";

    //     qDebug() << "Registration complete for user:" << username;
    //     qDebug() << "Token:" << token;
    //     qDebug() << "User ID:" << userId;



    //     signupButton->setVisible(false);
    //     loginButton->setVisible(false);
    //     logoutButton->setVisible(true);
    //     stackedWidget->setCurrentIndex(2);  // Индекс главной страницы
    //     emit userAuthenticated(token, userId);
    // }

    void handleRegistrationFailure(const QString &errorMessage) {
        qDebug() << "Registration failed:" << errorMessage;
    }

    void handleLoginFailure(const QString &errorMessage) {
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

    void sendGetCurrentDeals(const QString &token, const QString &userId) {
        qDebug() << "LogFunc: sendGetRequests()";
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::handleGetReplyCurrentDeals);

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
        qDebug() << "LogFunc: handleGetReply()";
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            QJsonObject jsonResponse = jsonDoc.object();
            displayDeals(jsonResponse);

            qDebug() << "Get request successful:" << jsonResponse;
        } else {

            qDebug() << "Get request failed:" << reply->errorString();
        }

        reply->deleteLater();
    }

void displayDeals(QJsonObject jsonDeals) {
    qDebug() << "LogFunc: displayDeals()";

    dealsList = new QListWidget;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(dealsList);
    setLayout(layout);

    QStringList keys = jsonDeals.keys();
    for (const QString &key : keys) {
        QJsonObject deal = jsonDeals[key].toObject();

        QString amount = deal["amount"].toString();
        QString dealStatus = deal["deal_status"].toString();
        QString dealType = deal["deal_type"].toString();
        QString exchangeRate = deal["exchange_rate"].toString();
        QString fromUserId = deal["from_user_id"].toString();
        QString id = deal["id"].toString();
        QString timeStart = deal["time_start"].toString();
        QString toUserId = deal["to_user_id"].toString();

        QString dealString = QString("Deal %1:\n"
                                    "Amount: %2\n"
                                    "Deal Status: %3\n"
                                    "Deal Type: %4\n"
                                    "Exchange Rate: %5\n"
                                    "From User ID: %6\n"
                                    "ID: %7\n"
                                    "Time Start: %8\n"
                                    "To User ID: %9\n")
                                .arg(key)
                                .arg(amount)
                                .arg(dealStatus)
                                .arg(dealType)
                                .arg(exchangeRate)
                                .arg(fromUserId)
                                .arg(id)
                                .arg(timeStart)
                                .arg(toUserId);

        dealsList->addItem(dealString);
    }
}


signals:
    void userAuthenticated(const QString &token, const QString &userId);

private:
    QPushButton *signupButton;
    QPushButton *loginButton;
    QPushButton *logoutButton;
    QStackedWidget *stackedWidget;
    QListWidget *dealsList;
};


int main(int argc, char **argv) {
    qDebug() << "LogFunc: main()";
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}

#include "client.moc"
