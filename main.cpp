#include "mainwindow.h"

#include <QGuiApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);
    QSurfaceFormat format;
    format.setSamples(16);
    MainWindow w;
    w.setFormat(format);
    w.resize(800, 600);
    w.show();
    return a.exec();
}
