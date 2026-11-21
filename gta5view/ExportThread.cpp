#include "SnapmaticPicture.h"
#include "ProfileInterface.h"
#include "PictureExport.h"
#include "ProfileWidget.h"
#include "ExportThread.h"
#include "SavegameData.h"
#include "AppEnv.h"
#include "config.h"
#include <QStringBuilder>
#include <QApplication>
#include <QFileInfo>
#include <QFile>
#if QT_VERSION >= 0x050000
#include <QScreen>
#else
#include <QDesktopWidget>
#endif
ExportThread::ExportThread(QMap<ProfileWidget*,QString> profileMap, QString exportDirectory, bool pictureCopyEnabled, bool pictureExportEnabled, int exportCount, QObject *parent) : QThread(parent),
    profileMap(profileMap), exportDirectory(exportDirectory), pictureCopyEnabled(pictureCopyEnabled), pictureExportEnabled(pictureExportEnabled), exportCount(exportCount)
{
}
void ExportThread::run()
{
    QSettings settings(GTA5SYNC_APPVENDOR, GTA5SYNC_APPSTR);
    settings.beginGroup("Pictures");
    int defaultQuality = 100;
    QSize defExportSize = SnapmaticPicture::getSnapmaticResolution();
    int customQuality = settings.value("CustomQuality", defaultQuality).toInt();
    if (customQuality < 1 || customQuality > 100)
    {
        customQuality = 100;
    }
    bool useCustomQuality = settings.value("CustomQualityEnabled", false).toBool();
    QSize cusExportSize = settings.value("CustomSize", defExportSize).toSize();
    if (cusExportSize.width() > 3840)
    {
        cusExportSize.setWidth(3840);
    }
    else if (cusExportSize.height() > 2160)
    {
        cusExportSize.setHeight(2160);
    }
    if (cusExportSize.width() < 1)
    {
        cusExportSize.setWidth(1);
    }
    else if (cusExportSize.height() < 1)
    {
        cusExportSize.setHeight(1);
    }
    QString sizeMode = settings.value("ExportSizeMode", "Default").toString();
    Qt::AspectRatioMode aspectRatio = (Qt::AspectRatioMode)settings.value("AspectRatio", Qt::KeepAspectRatio).toInt();
    settings.endGroup();
    int intExportProgress = 0;
    for (ProfileWidget *widget : profileMap.keys())
    {
        if (widget->isSelected())
        {
            if (widget->getWidgetType() == "SnapmaticWidget")
            {
                SnapmaticWidget *picWidget = qobject_cast<SnapmaticWidget*>(widget);
                SnapmaticPicture *picture = picWidget->getPicture();
                if (pictureExportEnabled)
                {
                    QString exportFileName = PictureExport::getPictureFileName(picture);
                    if (exportFileName.right(4) != ".jpg" && exportFileName.right(4) != ".png")
                    {
                        exportFileName += ".jpg";
                    }
                    intExportProgress++;
                    emit exportStringUpdate(ProfileInterface::tr("Экспортировать файл %1 из %2 файлов").arg(QString::number(intExportProgress), QString::number(exportCount)));
                    emit exportProgressUpdate(intExportProgress);
                    QImage exportPicture = picture->getImage();
                    if (sizeMode == "Desktop")
                    {
#if QT_VERSION >= 0x050000
                        qreal screenRatioPR = AppEnv::screenRatioPR();
                        QRect desktopResolution = QApplication::primaryScreen()->geometry();
                        int desktopSizeWidth = qRound((double)desktopResolution.width() * screenRatioPR);
                        int desktopSizeHeight = qRound((double)desktopResolution.height() * screenRatioPR);
#else
                        QRect desktopResolution = QApplication::desktop()->screenGeometry();
                        int desktopSizeWidth = desktopResolution.width();
                        int desktopSizeHeight = desktopResolution.height();
#endif
                        exportPicture = exportPicture.scaled(desktopSizeWidth, desktopSizeHeight, aspectRatio, Qt::SmoothTransformation);
                    }
                    else if (sizeMode == "Custom")
                    {
                        exportPicture = exportPicture.scaled(cusExportSize, aspectRatio, Qt::SmoothTransformation);
                    }
                    bool isSaved;
                    if (useCustomQuality)
                    {
                        isSaved = exportPicture.save(exportDirectory % "/" % exportFileName, "JPEG", customQuality);
                    }
                    else
                    {
                        isSaved = exportPicture.save(exportDirectory % "/" % exportFileName, "JPEG", 100);
                    }
                    if (!isSaved)
                    {
                        failedExportPictures += exportFileName;
                    }
                }
                if (pictureCopyEnabled)
                {
                    QString exportFileName = PictureExport::getPictureFileName(picture);
                    if (exportFileName.right(4) != ".g5e")
                    {
                        exportFileName += ".g5e";
                    }
                    intExportProgress++;
                    emit exportStringUpdate(ProfileInterface::tr("Экспортировать файл %1 из %2 файлов").arg(QString::number(intExportProgress), QString::number(exportCount)));
                    emit exportProgressUpdate(intExportProgress);
                    QString exportFilePath = exportDirectory % "/" % exportFileName;
                    if (QFile::exists(exportFilePath)) {QFile::remove(exportFilePath);}
                    if (!picture->exportPicture(exportDirectory % "/" % exportFileName, SnapmaticFormat::G5E_Format))
                    {
                        failedCopyPictures += exportFileName;
                    }
                }
            }
            else if (widget->getWidgetType() == "SavegameWidget")
            {
                SavegameWidget *sgdWidget = qobject_cast<SavegameWidget*>(widget);
                SavegameData *savegame = sgdWidget->getSavegame();
                QString originalFileName = savegame->getSavegameFileName();
                QFileInfo originalFileInfo(originalFileName);
                QString exportFileName = originalFileInfo.fileName();
                intExportProgress++;
                emit exportStringUpdate(ProfileInterface::tr("Экспортировать файл %1 из %2 файлов").arg(QString::number(intExportProgress), QString::number(exportCount)));
                emit exportProgressUpdate(intExportProgress);
                QString exportFilePath = exportDirectory % "/" % exportFileName;
                if (QFile::exists(exportFilePath)) {QFile::remove(exportFilePath);}
                if (!QFile::copy(originalFileName, exportFilePath))
                {
                    failedSavegames += exportFileName;
                }
            }
        }
    }
    emit exportFinished();
}
QStringList ExportThread::getFailedCopyPictures()
{
    return failedCopyPictures;
}
QStringList ExportThread::getFailedExportPictures()
{
    return failedExportPictures;
}
QStringList ExportThread::getFailedSavegames()
{
    return failedSavegames;
}