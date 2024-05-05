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

// Función que permite conectarse al socket cuando se pulsa el botón correspondiente
void MainWindow::on_btnCnct_clicked()
{
    ui->txtFeedBack->clear();
    mySocket->connectToHost("127.0.0.1", 8080);
    ui->txtFeedBack->setText("Esperando para conectar con el socket.");
    if(mySocket->waitForConnected(50000)){
        ui->txtFeedBack->setText("Interfaz conectada al socket.");
    }
}

// Función que muestra un mensaje pasado como argumento en la caja de texto
void MainWindow::typeFeedback(QString str){
    ui->txtFeedBack->setText(str);
}

// Función que cierra la conexión con el socket cuando se pulsa el botón correspondiente
void MainWindow::on_btnClose_clicked()
{
    if(mySocket->isOpen()){
        mySocket->close();
        ui->txtFeedBack->clear();
        ui->txtFeedBack->setText("Conexión con el socket cerrada.");
    }
}

void MainWindow::readyRead(){
    QByteArray message;
    message.clear();
    message = mySocket->readAll();

    // Decodificación del mensaje recibido
    switch (message[0]) {
    case 't':{
        // Se elimina los dos primeros bytes para suprimir los caracteres "t " de antes del mensaje.
        if(message.size() >= 2){
            message.remove(0, 2);
            message.replace('\0', "");
        }

        // Conversión de bytes a QString
        QString messageAsString = QString(message);
        // Se envía el mensaje al feedback text
        ui->txtFeedBack->setText(messageAsString);
    }
        break;
    default:
        break;
    }
}

// Sliders de control de ángulo

void MainWindow::on_sliderPos1_valueChanged(int value)
{
    // Imprimir el mensaje en la caja lateral.
    float f_value = 0.0;
    f_value = static_cast<float>(value)/100.0f;
    ui->txtTargRev1->setText(QString::number(f_value));

    // Primero se comprubea si el socket está disponible.
    if(mySocket->isWritable()){
        // Mandar el mensaje al servidor.
        QByteArray send_message;
        QString str_value = "t1 ";
        str_value.append(QString::number(f_value));
        send_message = str_value.toUtf8();
        mySocket->write(send_message);
        mySocket->waitForBytesWritten(2000);
        send_message.clear();
    }
}


void MainWindow::on_sliderPos2_valueChanged(int value)
{
    float f_value = 0.0;
    f_value = static_cast<float>(value)/100.0f;
    ui->txtTargRev2->setText(QString::number(f_value));

    // Primero se comprubea si el socket está disponible.
    if(mySocket->isWritable()){
        // Mandar el mensaje al servidor.
        QByteArray send_message;
        QString str_value = "t2 ";
        str_value.append(QString::number(f_value));
        send_message = str_value.toUtf8();
        mySocket->write(send_message);
        mySocket->waitForBytesWritten(2000);
        send_message.clear();
    }
}

