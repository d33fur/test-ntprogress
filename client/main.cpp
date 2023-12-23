#include "client.h"
#include <QApplication>
#include <QWindow>
//#include <Windows.h>
int main(int argc, char *argv[]) {

  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
  qputenv("QT_SCALE_FACTOR", "1");

  QApplication app(argc, argv);
  QPalette palette = qApp->palette();
  palette.setColor(QPalette::Window, QColor("#272b2e"));
  qApp->setPalette(palette);


  Login window;
  window.resize(800, 600);

  window.setWindowTitle("PresetShare");
  window.show();
  return app.exec();
}