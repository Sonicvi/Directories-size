#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QBoxLayout>
#include <QLineEdit>
#include <QString>
#include <QMap>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QFileDialog>
#include <QColor>
#include <QDebug>
#include <QThread>

#include "cdirsizecalculation.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_CHARTS_USE_NAMESPACE
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

    QThread calculationThread;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    long getDirSize(QString path);
    DirDesc getDirFilesList(QString path);
    void createCalculation();
    void startCalculation();
    QString sizeToQString(long);

    Ui::MainWindow *ui;
    QChartView *chartView;
    QPieSeries *series;
    CDirSizeCalculation *calculation;

private slots:
    void onChoseButtonClicked();
    void onStopButtonClicked();
    void onCalculationFinished();
    void onCalculationFailed(QString error);
    void onMouseOverPieSlice(bool);
    void onPieSliceClicked();

signals:
    void operate();

};
#endif // MAINWINDOW_H
