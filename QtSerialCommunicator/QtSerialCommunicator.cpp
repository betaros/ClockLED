#include "QtSerialCommunicator.h"

// http://doc.qt.io/qt-5/qtserialport-terminal-example.html
/*
* Constructor
*/
QtSerialCommunicator::QtSerialCommunicator(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    getPorts();

    status = false;
    updateTextfield("Start");

    QObject::connect(ui.ButtonConnect, SIGNAL(pressed()), this, SLOT(connectSerial()));
    QObject::connect(ui.ButtonSend, SIGNAL(pressed()), this, SLOT(send()));
    QObject::connect(ui.ButtonExport, SIGNAL(pressed()), this, SLOT(exportOutput()));

    fillCheckboxes();
    QObject::connect(ui.cbSettingBaud, SIGNAL(currentIndexChanged(int)), this, SLOT(setBaudrate()));
    QObject::connect(ui.cbSettingDatabit, SIGNAL(currentIndexChanged(int)), this, SLOT(setDatabit()));
    QObject::connect(ui.cbSettingParity, SIGNAL(currentIndexChanged(int)), this, SLOT(setParity()));
    QObject::connect(ui.cbSettingStopbit, SIGNAL(currentIndexChanged(int)), this, SLOT(setStopbit()));

    QObject::connect(&qsp, &QSerialPort::readyRead, this, &QtSerialCommunicator::receive);

    ui.statusBar->showMessage("Disconnected");
}

// http://doc.qt.io/qt-5/qtserialport-creaderasync-example.html
/*
* Receive data from serial port
*/
void QtSerialCommunicator::receive() {
    QByteArray ba = qsp.readAll();
    updateTextfield(ba);
}

/*
* Send data to serial port
*/
void QtSerialCommunicator::send() {
    QString output;
    if (status) {
        QString text = ui.lineEditSendText->text();
        QByteArray ba = text.toLatin1();
        const char *buffer = ba.data();
        qsp.write(buffer);

        output = "send -> " + text;
    } else {
        output = "send -> not connected";
    }

    ui.lineEditSendText->clear();
    updateTextfield(output);
}

/*
* Get all available ports
*/
void QtSerialCommunicator::getPorts() {
    portlist = qspi.availablePorts();
    ui.comboBoxSerialPort->clear();
    for each (QSerialPortInfo qspi_temp in portlist)
    {
        QString text = qspi_temp.portName() + " | " + qspi_temp.manufacturer();
        ui.comboBoxSerialPort->addItem(text);
    }
}

/*
* Slot for Connect Button
* Connects to choosen serial port
*/
void QtSerialCommunicator::connectSerial() {
    if (status == false) {
        // Connect to device
        qsp.setPort(portlist.at(ui.comboBoxSerialPort->currentIndex()));
        status = qsp.open(QIODevice::ReadWrite);
        if (status) {
            // Connection successful
            QString settings;
            updateTextfield("Connected");
            ui.statusBar->showMessage("Connected");
            ui.ButtonConnect->setText("Disconnect");
        } else {
            // Connection failed
            updateTextfield("Connection failed");
            ui.statusBar->showMessage("Connection failed");
        }
    } else {
        // Disconnect from device
        if (qsp.isOpen()) {
            qsp.close();
            status = false;
        }
        if (!status) {
            updateTextfield("Disconnected");
            ui.statusBar->showMessage("Disconnected");
            ui.ButtonConnect->setText("Connect");
        } else {
            updateTextfield("Disconnection failed");
            ui.statusBar->showMessage("Disconnection failed");
        }
    }
}

/*
* Updates text in textfield
*/
void QtSerialCommunicator::updateTextfield(QString text) {
    text = text.simplified();
    outputlist.push_back(text);
    ui.plainTextEditOutput->appendPlainText(text);
}

/*
* Export output to file
*/
void QtSerialCommunicator::exportOutput() {
    QString filename = "output.txt";
    QFile file(filename);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        for each (QString text in outputlist)
        {
            stream << text << endl;
        }
        file.close();
    }
    ui.statusBar->showMessage("Exported to " + file.fileName());
}

// Settings
// http://doc.qt.io/qt-5/qserialport.html

/* fills the checkboxes */
void QtSerialCommunicator::fillCheckboxes() {
    ui.cbSettingBaud->addItems(baud);
    ui.cbSettingBaud->setCurrentIndex(3);
    setBaudrate();

    ui.cbSettingDatabit->addItems(databit);
    ui.cbSettingDatabit->setCurrentIndex(3);
    setDatabit();

    ui.cbSettingParity->addItems(parity);
    ui.cbSettingParity->setCurrentIndex(0);
    setParity();

    ui.cbSettingStopbit->addItems(stopbit);
    ui.cbSettingStopbit->setCurrentIndex(0);
    setStopbit();

    ui.cbSettingFlowcontrol->addItems(flowcontrol);
    ui.cbSettingFlowcontrol->setCurrentIndex(0);
    setFlowcontrol();
}

/* Baudrate */
void QtSerialCommunicator::setBaudrate() {
    int i = ui.cbSettingBaud->currentIndex();
    switch (i) {
    case 0:
        qsp.setBaudRate(QSerialPort::Baud1200);
        break;
    case 1:
        qsp.setBaudRate(QSerialPort::Baud2400);
        break;
    case 2:
        qsp.setBaudRate(QSerialPort::Baud4800);
        break;
    case 4:
        qsp.setBaudRate(QSerialPort::Baud19200);
        break;
    case 5:
        qsp.setBaudRate(QSerialPort::Baud38400);
        break;
    case 6:
        qsp.setBaudRate(QSerialPort::Baud57600);
        break;
    case 7:
        qsp.setBaudRate(QSerialPort::Baud115200);
        break;
    default:
        qsp.setBaudRate(QSerialPort::Baud9600);
    }
}

/* Databit */
void QtSerialCommunicator::setDatabit() {
    int i = ui.cbSettingDatabit->currentIndex();
    switch (i) {
    case 0:
        qsp.setDataBits(QSerialPort::Data5);
        break;
    case 1:
        qsp.setDataBits(QSerialPort::Data6);
        break;
    case 2:
        qsp.setDataBits(QSerialPort::Data7);
        break;
    default:
        qsp.setDataBits(QSerialPort::Data8);
    }
}

/* Parity */
void QtSerialCommunicator::setParity() {
    int i = ui.cbSettingParity->currentIndex();
    switch (i) {
    case 1:
        qsp.setParity(QSerialPort::EvenParity);
        break;
    case 2:
        qsp.setParity(QSerialPort::OddParity);
        break;
    case 3:
        qsp.setParity(QSerialPort::SpaceParity);
        break;
    case 4:
        qsp.setParity(QSerialPort::MarkParity);
        break;
    default:
        qsp.setParity(QSerialPort::NoParity);
    }
}

/* Stopbit */
void QtSerialCommunicator::setStopbit() {
    int i = ui.cbSettingStopbit->currentIndex();
    switch (i) {
    case 1:
        qsp.setStopBits(QSerialPort::OneAndHalfStop);
        break;
    case 2:
        qsp.setStopBits(QSerialPort::TwoStop);
        break;
    default:
        qsp.setStopBits(QSerialPort::OneStop);
    }
}

/* Stopbit */
void QtSerialCommunicator::setFlowcontrol() {
    int i = ui.cbSettingFlowcontrol->currentIndex();
    switch (i) {
    case 1:
        qsp.setFlowControl(QSerialPort::HardwareControl);
        break;
    case 2:
        qsp.setFlowControl(QSerialPort::SoftwareControl);
        break;
    default:
        qsp.setFlowControl(QSerialPort::NoFlowControl);
    }
}
