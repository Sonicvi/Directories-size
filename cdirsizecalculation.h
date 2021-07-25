#ifndef CDIRSIZECALCULATION_H
#define CDIRSIZECALCULATION_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QDebug>
#include <QThread>
#include <boost/filesystem.hpp>

typedef struct {
    long size;
    QMap<QString,long> file;
    QMap<QString,long> dir;
}DirDesc;

class CDirSizeCalculation : public QObject
{
    Q_OBJECT

public:
    explicit CDirSizeCalculation(QObject *parent = nullptr);

    void setPath(const QString &path);

    QString getPath() const;

    DirDesc getResult() const;

private:
    QString m_path;
    DirDesc m_result;

    long getDirSize(QString path);
    DirDesc getDirFilesList(QString path);

public slots:
    void runCalculation();

signals:
    void resultReady();
    void errorOccured(QString error);


};

#endif // CDIRSIZECALCULATION_H
