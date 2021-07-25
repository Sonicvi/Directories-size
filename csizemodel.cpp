#include "csizemodel.h"

CSizeModel::CSizeModel()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(":memory:");
//    m_db.setDatabaseName("C:\\Users\\loicv\\Documents\\Programmation\\Directories-size\\db.db");

    if (!m_db.open()) {
        qDebug()<<"error ouverture db";

    }

    QSqlQuery query;
    query.exec("create table directories (id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT, type varchar(20), path TEXT, directory INTEGER, size INTEGER, sizeText TEXT)");

    query.exec("create table files (id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                             "name TEXT,"
                                             "type varchar(20),"
                                             "path TEXT,"
                                             "directory INTEGER,"
                                             "size INTEGER,"
                                             "sizeText TEXT)");


}

void CSizeModel::insertFile(QString filename, QString type, QString path, int directory, size_t size)
{
    QSqlQuery query(m_db);
    query.prepare("insert into files (name, type, path, directory, size, sizeText) values (?,?,?,?,?,?)");
    query.addBindValue(filename);
    query.addBindValue(type);
    query.addBindValue(path);
    query.addBindValue(directory);
    query.addBindValue(size);
    query.addBindValue(sizeToQString(size));


    QString e = query.lastError().text();

    query.exec();
}

int CSizeModel::insertDirectory(QString name, QString path, int directory)
{
    QSqlQuery query(m_db);
    query.prepare("insert into directories (name, type, path, directory) values (?,?,?,?)");
    query.addBindValue(name);
    query.addBindValue("dir");
    query.addBindValue(path);
    query.addBindValue(directory);
    query.exec();

    QString e = query.lastError().text();

    return query.lastInsertId().toInt();
}

void CSizeModel::setDirectorySize(int directory, size_t size)
{
   QSqlQuery query(m_db);
   bool b = query.exec("UPDATE directories SET size = "+QString::number(size)+" , sizeText = \""+ sizeToQString(size) +"\" WHERE id = "+QString::number(directory));
//   query.addBindValue(size);
//   query.addBindValue(directory);
//   bool b = query.exec();
   qDebug()<<"-------- lastQuery : " << query.lastQuery() <<"     b : " << b << "    size : "<< size << "   id : " << directory << "    error : " <<query.lastError().text();
}

QSqlQueryModel* CSizeModel::getModel()
{
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery("SELECT d.name as Name , d.type as Type ,d.sizeText  as Size FROM directories d WHERE d.directory = 1 UNION SELECT f.name as Name , f.type as Type,f.sizeText as Size FROM files f WHERE  f.directory = 1");
//    model->setHeaderData(0, Qt::Horizontal, "Name");
//    model->setHeaderData(1, Qt::Horizontal, "Size");
    QString salary = model->data(model->index(0,0)).toString();
//    QSqlError e = model->lastError();
    qDebug()<< model->lastError().text();
    return model;
}

QString CSizeModel::sizeToQString(size_t size)
{
    long double d_size = size;
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
