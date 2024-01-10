#include "registrationForm.h"

RegistrationForm::RegistrationForm(QWidget *parent) : QWidget(parent) {
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

void RegistrationForm::registerClicked() {
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
    qDebug() << "user input data" << json;

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &RegistrationForm::handleNetworkReply);

    QNetworkRequest request(QUrl("http://localhost:8001/signup"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    manager->post(request, data);
}

void RegistrationForm::handleNetworkReply(QNetworkReply *reply) {
    qDebug() << "LogFunc: handleNetworkReply()";

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonResponse = jsonDoc.object();

        if (jsonResponse.contains("token") && jsonResponse.contains("user_id")) {
            QString token = jsonResponse["token"].toString();
            QString userId = jsonResponse["user_id"].toString();

            emit registrationComplete(usernameEdit->text(), token, userId);
        } 
        else if (jsonResponse.contains("User already exists")){
            emit registrationFailed("User already exists");
        } 
        else {
            emit registrationFailed("something goes wrong with backend");
        }
    } 
    else {
        emit registrationFailed(reply->errorString());
    }

    reply->deleteLater();
}

// #include "registrationForm.moc"