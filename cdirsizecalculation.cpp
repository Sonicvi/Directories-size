#include "cdirsizecalculation.h"

namespace bf=boost::filesystem;

CDirSizeCalculation::CDirSizeCalculation(CSizeModel* model ,QObject *parent):m_model(model) ,m_path("")
{

}

void CDirSizeCalculation::runCalculation()
{
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

size_t CDirSizeCalculation::getDirSize(bf::path path, int dirId)
{
    size_t size=0;
    try
    {
        for(bf::recursive_directory_iterator it(path, bf::directory_options::skip_permission_denied);
            it!=bf::recursive_directory_iterator();
            ++it)
        {
            if(!bf::is_directory(*it)){
                QString name = QString::fromWCharArray(it->path().filename().c_str());
                QString ext  = QString::fromWCharArray(it->path().extension().c_str());
                QString path = QString::fromWCharArray(it->path().c_str());
                size_t fileSize = bf::file_size(*it);
                qDebug()<<"- file : "<< name <<" || size : "<<fileSize ;
                size+=fileSize;
                m_model->insertFile(name, ext, path, dirId, fileSize);
            }

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
    QString name = QString::fromWCharArray(bf::path(path.toStdString()).filename().c_str());
    int orgDir = m_model->insertDirectory(name, path, -1);
    for(bf::directory_iterator it(path.toStdString(), bf::directory_options::skip_permission_denied);
        it!=bf::directory_iterator();
        ++it)
    {
        if(!bf::is_directory(*it))
        {
            QString name = QString::fromWCharArray(it->path().filename().c_str());
            QString ext  = QString::fromWCharArray(it->path().extension().c_str());
            QString path = QString::fromWCharArray(it->path().c_str());
            size_t fileSize = bf::file_size(*it);
            m_model->insertFile(name, ext, path, orgDir, fileSize);
            ret.file[name] = fileSize;
            size+=fileSize;
        }
        else if(!bf::is_symlink(*it))
        {
            QString name = QString::fromWCharArray(it->path().filename().c_str());
            QString path = QString::fromWCharArray(it->path().c_str());
            int dirId = m_model->insertDirectory(name,path,orgDir);
            size_t dir_size = getDirSize(it->path(), dirId);
            m_model->setDirectorySize(dirId, dir_size);
            if(dir_size == ULLONG_MAX)
            {
                ret.size = ULLONG_MAX;
                return ret;
            }
            ret.dir[name] = dir_size;
            size+=ret.dir[name];
        }
    }
    m_model->setDirectorySize(orgDir, size);
    ret.size = size;
    return ret;
}
