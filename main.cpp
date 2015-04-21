#include "expertsdr_va2_1.h"
#include <QtGlobal>
#include <QApplication>
#include <QFont>
#include <QSplashScreen>
#include <QTranslator>
#include "Logger/VLogger.h"

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
//For qt4 can be set at ~/.config/Trolletch.conf and add nex three lines
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

    if(!QGLFormat::hasOpenGL() || !(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_2))
        {
            QMessageBox::information(0, "OpenGL 1.2 ERROR!", "This system does not support OpenGL 1.2!");
            qCritical() << "qApp: system does not support OpenGL 1.2!";
            return -1;
        }

    QTranslator translator(0);
        if(translator.load( QString("ExpertSDR_ru"), "." ))
            a.installTranslator(&translator);
        else
            qWarning() << "qApp: set translation file - file is not exist!";


    QPixmap pixmap(":images/splash.png");
    QSplashScreen splash(pixmap);
    splash.show();
    a.processEvents();

    ExpertSDR_vA2_1 w;
    w.show();
    splash.finish(&w);

    return a.exec();
}
