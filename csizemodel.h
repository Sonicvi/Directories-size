#ifndef CSIZEMODEL_H
#define CSIZEMODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include <QSqlQuery>
#include <QString>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QDebug>

class CSizeModel //: public QAbstractTableModel
{
public:
    CSizeModel();
    void insertFile(QString filename, QString type, QString path, int directory, size_t size);
    int insertDirectory(QString name, QString path, int directory);
    void setDirectorySize(int directory, size_t size);
    QSqlQueryModel* getModel();
    static QString sizeToQString(size_t);

private:
    QSqlDatabase m_db;

};

#endif // CSIZEMODEL_H
