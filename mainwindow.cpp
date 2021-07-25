#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/filesystem.hpp>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    chartView = new QChartView();
    series = new QPieSeries();
    ui->setupUi(this);
    ui->horizontalLayout->addWidget(chartView);
    connect(ui->pushButton,&QPushButton::clicked,this,&MainWindow::onButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

long MainWindow::getDirSize(QString path)
{
    namespace bf=boost::filesystem;
    size_t size=0;
    for(bf::recursive_directory_iterator it(path.toStdString());
        it!=bf::recursive_directory_iterator();
        ++it)
    {
        if(!bf::is_directory(*it)){
            QString test = QString::fromStdString(it->path().filename().string());
            qDebug()<<"file : "<< test <<" || size : "<<bf::file_size(*it) ;
            size+=bf::file_size(*it);}
    }
    qDebug()<<size;
    return size;
}

DirDesc MainWindow::getDirFilesList(QString path)
{
    namespace bf=boost::filesystem;
    size_t size=0;

    DirDesc ret;

    for(bf::directory_iterator it(path.toStdString());
        it!=bf::directory_iterator();
        ++it)
    {
        if(!bf::is_directory(*it))
        {
            QString name = QString::fromStdString(it->path().filename().string());
            ret.file[name] = bf::file_size(*it);
            size+=bf::file_size(*it);
        }
        else
        {
            QString name = QString::fromStdString(it->path().filename().string());
            ret.file[name] = getDirSize(QString::fromStdString(it->path().string()));
            size+=ret.file[name];
        }
    }
    ret.size = size;
    return ret;
}

void MainWindow::onButtonClicked()
{

    DirDesc test = getDirFilesList(ui->lineEdit->text());
    series->clear();

    ui->lineEdit_2->setText(QString::number(test.size));
    ui->textEdit->clear();
    ui->textEdit->setText("NAME             ||SIZE");
    ui->textEdit->append("_______________________________________");
    for(QMap<QString,long>::iterator it = test.file.begin(); it != test.file.end(); it++)
    {
        ui->textEdit->append(it.key() + "   ||    " + QString::number(it.value()));
        series->append(it.key(), it.value());
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->legend()->hide();
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
}

