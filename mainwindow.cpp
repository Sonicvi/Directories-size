#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,calculation(new CDirSizeCalculation())
{
    createCalculation();
    calculationThread.start();

    chartView = new QChartView();
    series = new QPieSeries();
    ui->setupUi(this);
    ui->hl_PieChartLayout->addWidget(chartView);
    connect(ui->pb_ChoseDir,&QPushButton::clicked,this,&MainWindow::onChoseButtonClicked);
    connect(ui->pb_Stop,&QPushButton::clicked,this,&MainWindow::onStopButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete calculation;
    delete chartView;
    delete series;
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
            ret.dir[name] = getDirSize(QString::fromStdString(it->path().string()));
            size+=ret.dir[name];
        }
    }
    ret.size = size;
    return ret;
}

void MainWindow::createCalculation()
{
    calculation = new CDirSizeCalculation();
    calculation->moveToThread(&calculationThread);
    //connect(&calculationThread, &QThread::finished, calculation, &QObject::deleteLater);
    connect(this,&MainWindow::operate,calculation,&CDirSizeCalculation::runCalculation);
    connect(calculation, &CDirSizeCalculation::resultReady, this, &MainWindow::onCalculationFinished);
}

void MainWindow::startCalculation()
{
    QString path = ui->le_Path->text();

    ui->te_Output->setTextColor(QColor("red"));
    ui->te_Output->setText("work in progress ...");

    ui->pb_Stop->setEnabled(true);
    ui->pb_ChoseDir->setEnabled(false);

    calculation->setPath(path);
    emit operate();
}

QString MainWindow::sizeToQString(long size)
{
    double d_size = size;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while(d_size >= 1024.0 && i.hasNext())
     {
        unit = i.next();
        d_size /= 1024.0;
    }
    return QString::number(d_size,'f',2)+" "+unit;
}

void MainWindow::onChoseButtonClicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    ui->le_Path->setText(path);

    startCalculation();
}

void MainWindow::onStopButtonClicked()
{
    calculationThread.terminate();
    calculationThread.wait();

    ui->te_Output->setTextColor(QColor("red"));
    ui->te_Output->setText("Calculation stopped");

    calculationThread.start();

    ui->pb_Stop->setEnabled(false);
    ui->pb_ChoseDir->setEnabled(true);

}

void MainWindow::onCalculationFinished()
{

    ui->pb_Stop->setEnabled(false);
    ui->pb_ChoseDir->setEnabled(true);

    DirDesc dir = calculation->getResult();

    series->clear();

    ui->le_Size->setText(sizeToQString(dir.size));

    ui->te_Output->clear();
    ui->te_Output->setTextColor(QColor("black"));
    ui->te_Output->setText("NAME             ||SIZE");
    ui->te_Output->append("_______________________________________");
    ui->te_Output->setTextColor(QColor("green"));

    for(QMap<QString,long>::iterator it = dir.dir.begin(); it != dir.dir.end(); it++)
    {
        ui->te_Output->append(it.key() + "   ||    " + sizeToQString(it.value()));
        series->append(it.key(), it.value());
        connect(series->slices().last(),&QPieSlice::hovered,this,&MainWindow::onMouseOverPieSlice);
        connect(series->slices().last(),&QPieSlice::clicked,this,&MainWindow::onPieSliceClicked);
    }

    long fileSize = 0;
    ui->te_Output->setTextColor(QColor("blue"));
    for(QMap<QString,long>::iterator it = dir.file.begin(); it != dir.file.end(); it++)
    {
        ui->te_Output->append(it.key() + "   ||    " + sizeToQString(it.value()));
        fileSize += it.value();
    }
    series->append("Files", fileSize);
    connect(series->slices().last(),&QPieSlice::hovered,this,&MainWindow::onMouseOverPieSlice);
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->legend()->hide();
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::onCalculationFailed(QString error)
{

    ui->te_Output->setTextColor(QColor("red"));
    ui->te_Output->setText("Error during calculation");
    ui->te_Output->append(error);

}

void MainWindow::onMouseOverPieSlice(bool hovered)
{
    QPieSlice *slice =  static_cast<QPieSlice*>(QObject::sender());

    slice->setExploded(hovered);
    slice->setLabelVisible(hovered);

}

void MainWindow::onPieSliceClicked()
{
    QPieSlice *slice =  static_cast<QPieSlice*>(QObject::sender());

    QString path = ui->le_Path->text() + "\\" + slice->label();
    ui->le_Path->setText(path);

    startCalculation();
}

