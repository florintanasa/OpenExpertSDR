#include "expertsdr_va2_1.h"
#include <QApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//Set icon to application
    QFile way(":MainIcon.ico");
    if(!way.open(QIODevice::ReadOnly))
    {
        qDebug()<<"not able to open icon";
    }

    QIcon icon(":MainIcon.ico");
    a.setWindowIcon(icon);

//I set font for qt5
//For qt4 can be set at ~/.config/Trolletch.conf
//[Qt]
//style=Fusion or Windows or GTK+
//font="Times New Roman,9,-1,5,50,0,0,0,0,0"
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QFile res(":images/font/Times_New_Roman.ttf");
    if(!res.open(QIODevice::ReadOnly))
    {
        qDebug()<<"not able to open font";
    }
    QFont myfont;
    myfont.setFamily("Times New Roman");
    myfont.setPointSize(9);
    a.setFont(myfont);
#endif

    ExpertSDR_vA2_1 w;
    w.show();

    return a.exec();
}
