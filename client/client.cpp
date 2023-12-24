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

private slots:
    void registerClicked() {
        qDebug() << "LogFunc: registerClicked()";
        QString firstName = firstNameEdit->text();
        QString lastName = lastNameEdit->text();
        QString username = usernameEdit->text();
        QString password = passwordEdit->text();
        bool rememberMe = rememberMeCheckBox->isChecked();


        QJsonObject json;
        json["first_name"] = firstName;
        json["second_name"] = lastName;
        json["login"] = username;
        json["password"] = password;
        json["remember_me"] = rememberMe ? "1" : "0";

        QJsonDocument jsonDoc(json);
        QByteArray data = jsonDoc.toJson();


        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, &QNetworkAccessManager::finished, this, &RegistrationForm::handleNetworkReply);

        QNetworkRequest request(QUrl("http://localhost:8001/signup"));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        manager->post(request, data);

        QString token = "";
        QString userId = "";
        emit registrationComplete(usernameEdit->text(), token, userId);
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
            } else {
                // Обработка других вариантов ответа

            }
        } else {
            // Обработка ошибки

        }

        reply->deleteLater();
    }

private:
    QLineEdit *firstNameEdit;
    QLineEdit *lastNameEdit;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
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

        manager->post(request, data);

        QString token = "";
        QString userId = "";
        emit loginComplete(usernameEdit->text(), token, userId);
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
            } else {
                // Обработка других вариантов ответа

            }
        } else {
            // Обработка ошибки

        }

        reply->deleteLater();
    }

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QCheckBox *rememberMeCheckBox;
};



class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr) : QWidget(parent) {
        qDebug() << "LogFunc: MainWindow()";
        QPushButton *signupButton = new QPushButton("Signup");
        QPushButton *loginButton = new QPushButton("Login");


        stackedWidget = new QStackedWidget;

        RegistrationForm *registrationForm = new RegistrationForm;
        LoginForm *loginForm = new LoginForm;

        stackedWidget->addWidget(registrationForm);
        stackedWidget->addWidget(loginForm);

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(signupButton);
        layout->addWidget(loginButton);
        layout->addWidget(stackedWidget);

        setLayout(layout);

        connect(signupButton, &QPushButton::clicked, this, &MainWindow::showRegistrationForm);
        connect(loginButton, &QPushButton::clicked, this, &MainWindow::showLoginForm);
        connect(registrationForm, &RegistrationForm::registrationComplete, this, &MainWindow::handleRegistrationComplete);
        connect(loginForm, &LoginForm::loginComplete, this, &MainWindow::handleLoginComplete);
        connect(this, &MainWindow::userAuthenticated, this, &MainWindow::sendGetRequests);
    }

private slots:
    void showRegistrationForm() {
        qDebug() << "LogFunc: showRegistrationForm()";
        stackedWidget->setCurrentIndex(0);  // Индекс формы регистрации
    }

    void showLoginForm() {
        qDebug() << "LogFunc: showLoginForm()";
        stackedWidget->setCurrentIndex(1);  // Индекс формы входа
    }

    void handleRegistrationComplete(const QString &username, const QString &token, const QString &userId) {
        qDebug() << "LogFunc: handleRegistrationComplete()";

        qDebug() << "Registration complete for user:" << username;
        qDebug() << "Token:" << token;
        qDebug() << "User ID:" << userId;

        // Дополнительные действия, например, переключение на главную страницу



        stackedWidget->setCurrentIndex(2);  // Индекс главной страницы
        emit userAuthenticated(token, userId);
    }

    void handleLoginComplete(const QString &username, const QString &token, const QString &userId) {
        qDebug() << "LogFunc: handleLoginComplete()";

        qDebug() << "Login complete for user:" << username;
        qDebug() << "Token:" << token;
        qDebug() << "User ID:" << userId;

        // Дополнительные действия, например, переключение на главную страницу



        stackedWidget->setCurrentIndex(2);  // Индекс главной страницы
        emit userAuthenticated(token, userId);
    }

    void sendGetRequests(const QString &token, const QString &userId) {
        qDebug() << "LogFunc: sendGetRequests()";
        sendGetRequest("http://localhost:8001/getCurrentDeals", token);
        sendGetRequestWithUserId("http://localhost:8001/getMyDeals", token, userId);
        sendGetRequestWithUserId("http://localhost:8001/getMyInfo", token, userId);
        sendGetRequestWithUserId("http://localhost:8001/getCurrencyQuotes", token, userId);

        displayDeals();
    }

    void sendGetRequest(const QString &url, const QString &token) {
        qDebug() << "LogFunc: sendGetRequest()";
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::handleGetReply);

        QNetworkRequest request;
        request.setUrl(QUrl(url));
        request.setRawHeader("Token", token.toUtf8());

        manager->get(request);
    }

    void sendGetRequestWithUserId(const QString &url, const QString &token, const QString &userId) {
        qDebug() << "LogFunc: sendGetRequestWithUserId()";
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::handleGetReply);

        QNetworkRequest request;
        request.setUrl(QUrl(url));
        request.setRawHeader("Token", token.toUtf8());
        request.setRawHeader("User_id", userId.toUtf8());

        manager->get(request);
    }

    void handleGetReply(QNetworkReply *reply) {
        qDebug() << "LogFunc: handleGetReply()";
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            QJsonObject jsonResponse = jsonDoc.object();


            qDebug() << "Get request successful:" << jsonResponse;
        } else {

            qDebug() << "Get request failed:" << reply->errorString();
        }

        reply->deleteLater();
    }

    void displayDeals() {
        qDebug() << "LogFunc: displayDeals()";

        dealsList = new QListWidget;
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(dealsList);
        setLayout(layout);


        dealsList->addItem("Deal 1");
        dealsList->addItem("Deal 2");
        dealsList->addItem("Deal 3");

    }

signals:
    void userAuthenticated(const QString &token, const QString &userId);

private:
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
