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
        if(ULLONG_MAX != m_result.size)
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

size_t CDirSizeCalculation::getDirSize(bf::path path)
{
    size_t size=0;
    try
    {
        for(bf::recursive_directory_iterator it(path, bf::directory_options::skip_permission_denied);
            it!=bf::recursive_directory_iterator();
            ++it)
        {
            if(!bf::is_directory(*it)){
                QString test = QString::fromStdString(it->path().filename().string());
                qDebug()<<"- file : "<< test <<" || size : "<<bf::file_size(*it) ;
                size+=bf::file_size(*it);}
        }
    }
    catch (const std::exception& e)
    {
        qDebug()<< e.what();
        qDebug()<< bf::status(path).permissions();
        qDebug()<< "error trying to get size of " + QString::fromWCharArray(path.c_str());

        emit errorOccured("error trying to get size of " + QString::fromWCharArray(path.c_str()));
        size = ULLONG_MAX;
    }
    return size;
}

DirDesc CDirSizeCalculation::getDirFilesList(QString path)
{
    size_t size=0;

    DirDesc ret;

    for(bf::directory_iterator it(path.toStdString(), bf::directory_options::skip_permission_denied);
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
            QString name = QString::fromWCharArray(it->path().filename().c_str());
            size_t dir_size = getDirSize(it->path());
            if(dir_size == ULLONG_MAX)
            {
                ret.size = ULLONG_MAX;
                return ret;
            }
            ret.dir[name] = dir_size;
            size+=ret.dir[name];
        }
    }
    ret.size = size;
    return ret;
}
