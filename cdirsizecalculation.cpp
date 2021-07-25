#include "cdirsizecalculation.h"

namespace bf=boost::filesystem;

CDirSizeCalculation::CDirSizeCalculation(QObject *parent):m_path("")
{

}

void CDirSizeCalculation::runCalculation()
{
    qDebug() << QThread::currentThreadId();
    if(m_path != "")
    {
        m_result = getDirFilesList(m_path);
        emit resultReady();
    }
    else
    {
        emit errorOccured("no path defined");
    }
}

void CDirSizeCalculation::setPath(const QString &path)
{
    m_path = path;
}

QString CDirSizeCalculation::getPath() const
{
    return m_path;
}

DirDesc CDirSizeCalculation::getResult() const
{
    return m_result;
}

long CDirSizeCalculation::getDirSize(QString path)
{
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

DirDesc CDirSizeCalculation::getDirFilesList(QString path)
{
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
