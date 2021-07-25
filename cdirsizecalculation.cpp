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
        if(-1 != m_result.size)
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

long CDirSizeCalculation::getDirSize(bf::path path)
{
    size_t size=0;
    try
    {
        qDebug()<< "dbt for";
        for(bf::recursive_directory_iterator it(path);
            it!=bf::recursive_directory_iterator();
            ++it)
        {
            qDebug()<< "test is directory";
            if(!bf::is_directory(*it)){
                qDebug()<< "get size";
                QString test = QString::fromStdString(it->path().filename().string());
                qDebug()<<"- file : "<< test <<" || size : "<<bf::file_size(*it) ;
                size+=bf::file_size(*it);}
        }
    }
    catch (const std::exception& e)
    {
        qDebug()<< e.what();

        emit errorOccured("error trying to get size of " + QString::fromWCharArray(path.c_str()));
        size = -1;
    }
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
        else if(!bf::is_symlink(*it))
        {
            QString::fromUtf8("éèçà");
            QString test = QString::fromWCharArray(it->path().filename().c_str( ));
            QString name = QString::fromWCharArray(it->path().filename().c_str());
            long dir_size = getDirSize(it->path());
            if(dir_size == -1)
            {
                ret.size = -1;
                return ret;
            }
            ret.dir[name] = dir_size;
            size+=ret.dir[name];
        }
    }
    ret.size = size;
    return ret;
}
