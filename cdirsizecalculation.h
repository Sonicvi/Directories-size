#ifndef CDIRSIZECALCULATION_H
#define CDIRSIZECALCULATION_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QDebug>
#include <QThread>
#include <boost/filesystem.hpp>
#include "csizemodel.h"

typedef struct {
    size_t size;
    QMap<QString,size_t> file;
    QMap<QString,size_t> dir;
}DirDesc;


namespace bf=boost::filesystem;

class CDirSizeCalculation : public QObject
{
    Q_OBJECT

public:
    explicit CDirSizeCalculation(CSizeModel* model ,QObject *parent = nullptr);

    void setPath(const QString &path);

    QString getPath() const;

    DirDesc getResult() const;

private:
    QString m_path;
    DirDesc m_result;
    CSizeModel* m_model;

    size_t getDirSize(bf::path path, int dirId);
    DirDesc getDirFilesList(QString path);

public slots:
    void runCalculation();

signals:
    void resultReady();
    void errorOccured(QString error);


};

#endif // CDIRSIZECALCULATION_H
