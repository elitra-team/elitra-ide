#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Elitra IDE");
    app.setApplicationVersion("1.0.0");

    MainWindow window;
    window.show();

    window.newFile();

    return app.exec();
}
