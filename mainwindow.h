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

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_CHARTS_USE_NAMESPACE
QT_END_NAMESPACE

typedef struct {
    long size;
    QMap<QString,long> file;
}DirDesc;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    long getDirSize(QString path);
    DirDesc getDirFilesList(QString path);
    Ui::MainWindow *ui;
    QChartView *chartView;
    QPieSeries *series;

private slots:
    void onButtonClicked();

};
#endif // MAINWINDOW_H
