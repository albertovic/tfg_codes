#include "mainwindow.h"
#include <netinet/in.h>
#include <unistd.h>
#include <QApplication>

using namespace std;
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
