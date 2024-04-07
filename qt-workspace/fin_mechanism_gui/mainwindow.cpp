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
    if(mySocket->isWritable()){
        QByteArray send_message;
        QString str_value = "s";
        send_message = str_value.toUtf8();
        mySocket->write(send_message);
        mySocket->waitForBytesWritten(2000);
        qDebug("Mensaje mandado");
    }
}


void MainWindow::on_btnCnct_clicked()
{
    ui->txtFeedBack->clear();
    mySocket->connectToHost("127.0.0.1", 8080);
    ui->txtFeedBack->setText("Esperando para conectar con el socket.");
    if(mySocket->waitForConnected(50000)){
        ui->txtFeedBack->setText("Interfaz conectada al socket.");
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
        ui->txtFeedBack->setText("Conexión con el socket cerrada.");
    }
}

void MainWindow::readyRead(){
    QByteArray message;
    // char messageAsChar[1024];
    // message = mySocket->readAll();
    // std::vector<double> div_message;
    QString messageAsString = QString(message);

    switch(message[0]){
    case 't':
        // Eliminamos los dos primeros bytes para eliminar la "t " de antes del mensaje.
        if(message.size() >= 2){
            message.remove(0, 2);
            message.replace('\0', "");
        }
        // Mandamos el mensaje al feedback text
        ui->txtFeedBack->setText(messageAsString);
        break;
    // case 'p':
    //     if(message.size() >= 2){
    //         message.remove(0, 2);
    //         message.replace('\0', "");
    //     }
    //     // Copiamos la información del mensaje y lo transformamos en un array de char
    //     memcpy(messageAsChar, message.constData(), message.size());
    //     // Utilizamos la función para dividir el mensaje

    //     while (ss >> parte)
    //     { // Leemos cada parte del stringstream
    //         double valor;
    //         if (std::istringstream(parte) >> valor)
    //         { // Intentamos convertir la parte en un double
    //             // Si la conversión es exitosa, agregamos el valor al vector resultado
    //             div_message.push_back(valor);
    //         }
    //     }

    //     ui->txtPosRev1->setText(QString::number(div_message[0]));
    //     ui->txtPosRev2->setText(QString::number(div_message[1]));
    default:
        break;
    }
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
    // Imprimir el mensaje en la caja lateral.
    float f_value = static_cast<float>(value)/100.0f;
    ui->txtTargRev1->setText(QString::number(f_value));

    // Mandar el mensaje al servidor.

    // Primero se comprubea si el socket está disponible.
    if(mySocket->isWritable()){
        QByteArray send_message;
        QString str_value = "t1 ";
        str_value.append(QString::number(f_value));
        send_message = str_value.toUtf8();
        mySocket->write(send_message);
        mySocket->waitForBytesWritten(2000);
    }
}


void MainWindow::on_sliderPos2_valueChanged(int value)
{
    float f_value = static_cast<float>(value)/100.0f;
    ui->txtTargRev2->setText(QString::number(f_value));

    // Mandar el mensaje al servidor.

    // Primero se comprubea si el socket está disponible.
    if(mySocket->isWritable()){
        QByteArray send_message;
        QString str_value = "t2 ";
        str_value.append(QString::number(f_value));
        send_message = str_value.toUtf8();
        mySocket->write(send_message);
        mySocket->waitForBytesWritten(2000);
    }
}
