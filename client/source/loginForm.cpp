#include "loginForm.h"

LoginForm::LoginForm(QWidget *parent) : QWidget(parent) {
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

void LoginForm::loginClicked() {
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

void LoginForm::handleNetworkReply(QNetworkReply *reply) {
    qDebug() << "LogFunc: handleNetworkReply()";

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonResponse = jsonDoc.object();

        if (jsonResponse.contains("token") && jsonResponse.contains("user_id")) {
            QString token = jsonResponse["token"].toString();
            QString userId = jsonResponse["user_id"].toString();

            emit loginComplete(usernameEdit->text(), token, userId);
        } 
        else if (jsonResponse.contains("Wrong login or password")) {
            emit loginFailed("Wrong login or password");
        } 
        else {
            emit loginFailed("something goes wrong");
        }
    } 
    else {
        emit loginFailed(reply->errorString());
    }

    reply->deleteLater();
}

// #include "loginForm.moc"