#include "mainWindow.h"
#include "dealDialog.h"
#include "registrationForm.h"
#include "loginForm.h"
#include "stockExchangeForm.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
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
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::showAuthForm);

    connect(registrationForm, &RegistrationForm::registrationComplete, this, &MainWindow::handleLoginComplete);
    connect(registrationForm, &RegistrationForm::registrationFailed, this, &MainWindow::handleRegistrationFailure);
    connect(loginForm, &LoginForm::loginComplete, this, &MainWindow::handleLoginComplete);
    connect(loginForm, &LoginForm::loginFailed, this, &MainWindow::handleLoginFailure);

    connect(this, &MainWindow::userAuthenticated, stockExchangeForm, &StockExchangeForm::setUserInfo);
}


void MainWindow::showRegistrationForm() {
    qDebug() << "LogFunc: showRegistrationForm()";

    stackedWidget->setVisible(true);
    stackedWidget->setCurrentIndex(0);  // Индекс формы регистрации
}

void MainWindow::showLoginForm() {
    qDebug() << "LogFunc: showLoginForm()";

    stackedWidget->setVisible(true);
    stackedWidget->setCurrentIndex(1);  // Индекс формы входа
}

void MainWindow::showAuthForm() {
    qDebug() << "LogFunc: showAuthFrom()";

    stackedWidget->setVisible(false);
    signupButton->setVisible(true);
    loginButton->setVisible(true);
    logoutButton->setVisible(false);
    stackedWidget->setCurrentIndex(0);  // Индекс формы регистрации
    //QApplication::quit();
}

void MainWindow::handleRegistrationFailure(const QString &errorMessage) {
    qDebug() << "LogFunc: handleRegistrationFailure()";

    qDebug() << "Registration failed:" << errorMessage;
}

void MainWindow::handleLoginFailure(const QString &errorMessage) {
    qDebug() << "LogFunc: handleLoginFailure()";

    qDebug() << "Login failed:" << errorMessage;
}

void MainWindow::handleLoginComplete(const QString &username, const QString &token, const QString &userId) {
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

// #include "mainWindow.moc"