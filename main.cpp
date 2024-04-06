#include "simbox.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SimBox w;
    w.show();
    return a.exec();
}
