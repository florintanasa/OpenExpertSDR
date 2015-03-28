#include "expertsdr_va2_1.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ExpertSDR_vA2_1 w;
    w.show();

    return a.exec();
}
