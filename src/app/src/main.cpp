#include "Orange.h"

#include <QApplication>
#pragma comment(lib, "user32.lib")

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Orange w;
    w.show();
    return a.exec();
}