#ifndef CDIRSIZECALCULATION_H
#define CDIRSIZECALCULATION_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QDebug>
#include <QThread>
#include <boost/filesystem.hpp>

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
    explicit CDirSizeCalculation(QObject *parent = nullptr);

    void setPath(const QString &path);

    QString getPath() const;

    DirDesc getResult() const;

private:
    QString m_path;
    DirDesc m_result;

    size_t getDirSize(bf::path path);
    DirDesc getDirFilesList(QString path);

public slots:
    void runCalculation();

signals:
    void resultReady();
    void errorOccured(QString error);


};

#endif // CDIRSIZECALCULATION_H
