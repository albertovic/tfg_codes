#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mySocket = new QTcpSocket(this);
    connect(mySocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnOnOff_clicked()
{

}


void MainWindow::on_btnCnct_clicked()
{
    ui->txtFeedBack->clear();
    mySocket->connectToHost("127.0.0.1", 8080);
    ui->txtFeedBack->setText("Waiting for socket to connect.");
    if(mySocket->waitForConnected(50000)){
        ui->txtFeedBack->setText("Connected to socket.");
    }
}

void MainWindow::typeFeedback(QString str){
    ui->txtFeedBack->setText(str);
}


void MainWindow::on_btnClose_clicked()
{
    if(mySocket->isOpen()){
        mySocket->close();
        ui->txtFeedBack->clear();
        ui->txtFeedBack->setText("Socket connection closed.");
    }
}

void MainWindow::readyRead(){
    QByteArray message;
    message = mySocket->readAll();
    // TODO: Aquí gestionar la forma de actuar dependiendo del mensaje.
}

// Sliders

void MainWindow::on_sliderPos1_valueChanged(int value)
{
    /** Cuando se mueve el Slider 1 se producen dos acciones.
            -La UI debe actualizar el valor mostrado en la caja de texto al lado del slider
            -El socket debe mandar el nuevo valor hasta el plugin de Gazebo
                *Recordar comprobar si está disponible el socket
                *Recordar recibir confirmación de lectura y mostrarla en el logger
    */
    float f_value = static_cast<float>(value)/100.0f;
    ui->txtTargRev1->setText(QString::number(f_value));
}


void MainWindow::on_sliderPos2_valueChanged(int value)
{
    float f_value = static_cast<float>(value)/100.0f;
    ui->txtTargRev2->setText(QString::number(f_value));
}

