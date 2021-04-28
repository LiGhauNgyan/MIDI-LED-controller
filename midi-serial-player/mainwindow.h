#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QList>
#include <QDebug>
#include <MidiFile.h>
#include <QTimer>
#include <MidiEvent.h>
#include <QFileDialog>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void stop_playing();
    void reset_progress();
    void set_ui_progress(double time);
    void on_timer_timeout();

private slots:
    void on_buttonPlay_clicked();

    void on_sliderSpeed_valueChanged(int value);

    void on_buttonOpenPort_clicked();

    void on_buttonLoadFile_clicked();

    void on_buttonStop_clicked();

    void on_bottonRefreshPortList_clicked();

    void on_comboBoxKey_currentIndexChanged(int index);

    void on_progressBar_sliderReleased();

    void on_progressBar_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    QList<QSerialPortInfo> portinfo;
    QSerialPort* pport;
    QTimer* timer;
    smf::MidiFile* mf;
    QVector<QPair<char, double>> el;
    int tracklen, eventindex;
    double currtime=0, totaltime=3000;
    double speed=5;
    int leftmostKey=24;
    const QStringList keyNames={"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
};

#endif // MAINWINDOW_H
