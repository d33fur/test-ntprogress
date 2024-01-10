#include <QApplication>
#include <QDebug>
#include "mainWindow.h"

int main(int argc, char **argv) {
    qDebug() << "LogFunc: main()";

    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}

// #include "main.moc"