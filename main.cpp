#define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING 
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#include "Header.h"

#include "mainwindow.h"
#include "rationcreatewindow.h"
#include <QLabel>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
