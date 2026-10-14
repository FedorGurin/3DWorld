#ifndef DATAPATH_H
#define DATAPATH_H

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QStringList>

//! поиск файла с данными (модели, рельеф) относительно каталога программы,
//! текущего каталога и каталога выше (типичная сборка в build-каталоге QtCreator)
inline QString findDataFile(const QString &relPath)
{
    if(QFileInfo(relPath).isAbsolute())
        return relPath;

    const QString appDir = QApplication::applicationDirPath();
    const QStringList dirs = {appDir,
                              QDir::currentPath(),
                              appDir + "/..",
                              QDir::currentPath() + "/.."};
    for(const QString &dir : dirs)
    {
        QString path = QDir::cleanPath(dir + "/" + relPath);
        if(QFileInfo::exists(path))
            return path;
    }
    return relPath;
}

#endif // DATAPATH_H
