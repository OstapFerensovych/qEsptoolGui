#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    enumerateCommPorts();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readyReadStandardOutput()
{
      QProcess *p = qobject_cast<QProcess*>(sender());
      p->setReadChannel(QProcess::StandardOutput);
      ui->esptoolOut->insertPlainText(p->readLine());
}

void MainWindow::finished(int,QProcess::ExitStatus)
{
    ui->uploadBtn->setEnabled(true);
    //ui->esptoolOut->append("Upload Finished.");
}
void MainWindow::on_fwOpenDialog_clicked()
{
    QString file1Name = QFileDialog::getOpenFileName(this, "Open Firmware", QDir::currentPath(), "BIN Files (*.bin)");
    ui->fwFileLoc->setText(file1Name);
}

void MainWindow::on_fsOpenDialog_clicked()
{
    QString file1Name = QFileDialog::getOpenFileName(this, "Open SPIFFS", QDir::currentPath(), "BIN Files (*.bin)");
    ui->fsFileLoc->setText(file1Name);
}

void MainWindow::enumerateCommPorts()
{
    ui->comPorts->clear();


    if(QSerialPortInfo::availablePorts().isEmpty())
    {
        ui->comPorts->addItem("There is no available devices!");
        ui->uploadBtn->setEnabled(false);
    }
    else
    {
        ui->uploadBtn->setEnabled(true);
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        {
            ui->comPorts->addItem(info.portName() + (info.description().isEmpty() ? "" : " - " + info.description()));
            #ifdef __linux__
            availComPorts.append("/dev/" + info.portName());
            #elif _WIN32
            availComPorts.append(info.portName());
            #endif

        }
    }
}

void MainWindow::showWarning(QString text)
{
    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.setWindowTitle("Warning");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
}

bool MainWindow::fileExists(QString path) {
    QFileInfo check_file(path);
    if (check_file.exists() && check_file.isFile()) {
        return true;
    } else {
        return false;
    }
}

bool MainWindow::isValid(){
    if(!fileExists(QDir::currentPath() + executive))
    {
        showWarning("No esptool.exe");
        return false;
    }

    if(!ui->fwCheckBox->isChecked() && !ui->fsCheckBox->isChecked())
    {
        showWarning("Nothing selected");
        return false;
    }

    if((ui->fwFileLoc->text().isEmpty() && ui->fwCheckBox->isChecked()) || (ui->fsFileLoc->text().isEmpty() && ui->fsCheckBox->isChecked()))
    {
        showWarning("Nothing to upload");
        return false;
    }

    if(ui->fwCheckBox->isChecked() && !fileExists(ui->fwFileLoc->text()))
    {
        showWarning("File: " + ui->fwFileLoc->text() + " do not exist");
        return false;
    }

    if(ui->fsCheckBox->isChecked() && !fileExists(ui->fsFileLoc->text()))
    {
        showWarning("File: " + ui->fsFileLoc->text() + " do not exist");
        return false;
    }

    return true;
}


void MainWindow::on_uploadBtn_clicked()
{
    if(!isValid()) return;

    QString cmd;

    cmd = QDir::currentPath() + executive;
    cmd.append(" -cp " + availComPorts[ui->comPorts->currentIndex()]);
    cmd.append(" -cd " + ui->resetMethod->currentText());
    cmd.append(" -cb " + ui->baudRate->currentText());
    if(ui->fwCheckBox->isChecked()){
        cmd.append(" -ca 0x" + ui->fwAddr->text());
        cmd.append(" -cf \"" + ui->fwFileLoc->text() + "\"");
    }
    if(ui->fsCheckBox->isChecked()){
        cmd.append(" -ca 0x" + ui->fsAddr->text());
        cmd.append(" -cf \"" + ui->fsFileLoc->text() + "\"");
    }

//    qDebug().noquote() << cmd;

    ui->esptoolOut->setText("");
    ui->uploadBtn->setEnabled(false);
    esptool->start(cmd);
    connect(esptool, SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadStandardOutput()));
    connect(esptool, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
    //ui->esptoolOut->append("Upload Started.\n");
}

void MainWindow::on_refreshComPorts_clicked()
{
    enumerateCommPorts();
}
