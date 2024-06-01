#include "simbox.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<Sample>();
    qRegisterMetaType<Results>();
    qRegisterMetaType<EngineParameters>();
    SimBox w;
    w.show();
    return a.exec();
}
