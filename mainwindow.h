#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QtSerialPort/QSerialPortInfo>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_fwOpenDialog_clicked();

    void on_fsOpenDialog_clicked();

    void on_uploadBtn_clicked();

    void readyReadStandardOutput();

    void finished(int, QProcess::ExitStatus);

    void on_refreshComPorts_clicked();

private:
    Ui::MainWindow *ui;

private:
    QProcess *esptool = new QProcess(this);
    void showWarning(QString text);
    bool isValid();
    bool fileExists(QString path);
    void enumerateCommPorts();
    QStringList availComPorts;
    #ifdef __linux__
        QString executive = "/esptool";
    #elif _WIN32
        QString executive = "/esptool.exe";
    #else
    #error "OS not supported."
    #endif
};

#endif // MAINWINDOW_H
