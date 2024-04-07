#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <iostream>
#include <vector>
#include <sstream>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    char lower_buffer[1024] = {0};
    int cnct_socket = 0;

    void typeFeedback(QString);

private slots:
    void on_btnOnOff_clicked();

    void on_btnCnct_clicked();

    void on_btnClose_clicked();

    void readyRead();

    void on_sliderPos1_valueChanged(int value);

    void on_sliderPos2_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    QTcpSocket *mySocket;
};
#endif // MAINWINDOW_H
