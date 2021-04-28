#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <algorithm>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    on_bottonRefreshPortList_clicked();
    pport=new QSerialPort();
    timer = new QTimer(this);
    timer->setInterval(5);
    timer->setSingleShot(false);
    tracklen=0;
    currtime=0;
    totaltime=1000;
    eventindex=0;
    connect(timer, &QTimer::timeout, this, &MainWindow::on_timer_timeout);
    for(int i=21; i<45; i++)
    {
        ui->comboBoxKey->addItem(keyNames[i%12] + QString::number(i/12-1), i);
    }
    leftmostKey=24;
    ui->comboBoxKey->setCurrentIndex(leftmostKey-21);
}

void MainWindow::on_timer_timeout()
{
    QByteArray msg(64, 0);
    int sendlen=0;
    currtime+=speed;
    for (;eventindex<tracklen;eventindex++) {
        if(el[eventindex].second<currtime)
        {
            msg[sendlen]=el[eventindex].first;
            sendlen++;
        }
        else {
            break;
        }
    }
    if(eventindex>=tracklen)
    {
        reset_progress();
    }
    if(sendlen)
    {
        if(pport->isOpen())
        {
            auto len=pport->write(msg,sendlen);
            if(!pport->waitForBytesWritten(5))
            {
                qDebug()<<"fail";
            }
        }
        else {
            qDebug()<<"port not open";
        }
    }
    set_ui_progress(currtime);
}

MainWindow::~MainWindow()
{
    delete ui;
    pport->close();
    delete pport;
}

void MainWindow::stop_playing()
{
    timer->stop();
    ui->buttonPlay->setText("Play");
    ui->progressBar->setEnabled(true);
}

void MainWindow::reset_progress()
{
    stop_playing();
    eventindex=0;
    currtime=0;
    set_ui_progress(0);
}

void MainWindow::set_ui_progress(double time)
{
    ui->progressBar->setValue(time/totaltime*1000);
}

void MainWindow::on_buttonPlay_clicked()
{
    if(timer->isActive())
    {
        stop_playing();
    }
    else
    {
        timer->start();
        ui->buttonPlay->setText("Pause");
        ui->progressBar->setEnabled(false);
    }

}

void MainWindow::on_sliderSpeed_valueChanged(int value)
{
    speed=value/20.0+1;
    ui->labelSpeed->setText(QString::number(speed/5));
}

void MainWindow::on_buttonOpenPort_clicked()
{
    if(pport->isOpen())
    {
        pport->clear();
        pport->close();
    }
    if(ui->comboBoxPort->currentData()!=0)
    {
        pport->setPortName(ui->comboBoxPort->currentText());
        if(!pport->open(QIODevice::ReadWrite))
        {
            qDebug()<<"open fail";
        }
        pport->setBaudRate(QSerialPort::Baud19200);
    }
}

void MainWindow::on_buttonLoadFile_clicked()
{
    reset_progress();
    QFileDialog dia(this);
    dia.setNameFilter("MIDI Files (*.mid)");
    QString path;
    if (dia.exec() == QDialog::Accepted)
    {
        path = dia.selectedFiles()[0];
        ui->labelFileName->setText(path);
    }
    else {
        return;
    }
    mf= new smf::MidiFile(path.toStdString());
    mf->doTimeAnalysis();
    el.clear();
    for(int i=0;i<mf->getTrackCount();i++)
    {
        auto& tr=(*mf)[i];
        for(int j=0;j<tr.size();j++)
        {
            if(tr[j].isNoteOn()&&tr[j].getKeyNumber()>=leftmostKey)
            {
                el.push_back({(char)(tr[j].getKeyNumber()-leftmostKey), 1000 * tr[j].seconds});
            }
        }
    }
    std::sort(el.begin(),
              el.end(),
              [](const QPair<char, double>& a, const QPair<char, double>& b)
    {
        return a.second < b.second;
    });
    tracklen=el.size();
    if(tracklen)
    {
        totaltime=el[tracklen-1].second+1000;
        int sec=(int)(totaltime/1000);
        ui->labelTotalTime->setText(QString("%1:%2").arg(sec/60).arg(sec%60,2,10,QLatin1Char('0')));
    }
    else
    {
        totaltime=1000;
        ui->labelTotalTime->setText("0:00");
    }
}

void MainWindow::on_buttonStop_clicked()
{
    reset_progress();
}

void MainWindow::on_bottonRefreshPortList_clicked()
{
    ui->comboBoxPort->clear();
    ui->comboBoxPort->addItem("Close", 0);
    portinfo=QSerialPortInfo::availablePorts();
    for(auto& port:portinfo)
    {
        ui->comboBoxPort->addItem(port.portName(),port.systemLocation());
    }
}

void MainWindow::on_comboBoxKey_currentIndexChanged(int index)
{
    leftmostKey=ui->comboBoxKey->currentData().toInt();
}

void MainWindow::on_progressBar_sliderReleased()
{
    stop_playing();
    currtime=totaltime*ui->progressBar->value()/1000.;
    for(int i=0;i<el.size();i++)
    {
        if(currtime<=el[i].second)
        {
            eventindex=i;
            break;
        }
    }
    set_ui_progress(currtime);
}

void MainWindow::on_progressBar_valueChanged(int value)
{
    int sec=(int)(value*totaltime/1000000.);
    ui->labelCurrentTime->setText(QString("%1:%2").arg(sec/60).arg(sec%60,2,10,QLatin1Char('0')));
}
