#include "config.h"
#include "AppEnv.h"
#include "PictureExport.h"
#include "PictureDialog.h"
#include "StandardPaths.h"
#include "SidebarGenerator.h"
#include <QStringBuilder>
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QDebug>
#if QT_VERSION < 0x050000
#include <QDesktopWidget>
#endif
#if QT_VERSION >= 0x050000
#include <QSaveFile>
#include <QScreen>
#endif
PictureExport::PictureExport()
{
}
void PictureExport::exportAsPicture(QWidget *parent, SnapmaticPicture *picture)
{
    QSettings settings(GTA5SYNC_APPVENDOR, GTA5SYNC_APPSTR);
    settings.beginGroup("Pictures");
    int defaultQuality = 100;
    QSize defExportSize = SnapmaticPicture::getSnapmaticResolution();
    int customQuality = settings.value("CustomQuality", defaultQuality).toInt();
    if (customQuality < 1 || customQuality > 100) {
        customQuality = 100;
    }
    bool useCustomQuality = settings.value("CustomQualityEnabled", false).toBool();
    QSize cusExportSize = settings.value("CustomSize", defExportSize).toSize();
    if (cusExportSize.width() > 3840) {
        cusExportSize.setWidth(3840);
    }
    else if (cusExportSize.height() > 2160) {
        cusExportSize.setHeight(2160);
    }
    if (cusExportSize.width() < 1) {
        cusExportSize.setWidth(1);
    }
    else if (cusExportSize.height() < 1) {
        cusExportSize.setHeight(1);
    }
    QString sizeMode = settings.value("ExportSizeMode", "Default").toString();
    Qt::AspectRatioMode aspectRatio = (Qt::AspectRatioMode)settings.value("AspectRatio", Qt::KeepAspectRatio).toInt();
    QString defaultExportFormat = settings.value("DefaultExportFormat", ".jpg").toString();
    settings.endGroup();
    settings.beginGroup("FileDialogs");
    bool dontUseNativeDialog = settings.value("DontUseNativeDialog", false).toBool();
    settings.beginGroup("ExportAsPicture");
fileDialogPreSave:
    QFileDialog fileDialog(parent);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setViewMode(QFileDialog::Detail);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setOption(QFileDialog::DontUseNativeDialog, dontUseNativeDialog);
    fileDialog.setOption(QFileDialog::DontConfirmOverwrite, true);
    fileDialog.setDefaultSuffix("suffix");
    fileDialog.setWindowFlags(fileDialog.windowFlags()^Qt::WindowContextHelpButtonHint);
    fileDialog.setWindowTitle(PictureDialog::tr("Экспортировать как изображение..."));
    fileDialog.setLabelText(QFileDialog::Accept, PictureDialog::tr("Export"));
    QStringList filters;
    filters << PictureDialog::tr("Графика JPEG (*.jpg *.jpeg)");
    filters << PictureDialog::tr("Переносимая сетевая графика (*.png)");
    fileDialog.setNameFilters(filters);
    QList<QUrl> sidebarUrls = SidebarGenerator::generateSidebarUrls(fileDialog.sidebarUrls());
    fileDialog.setSidebarUrls(sidebarUrls);
    fileDialog.setDirectory(settings.value("Directory", StandardPaths::picturesLocation()).toString());
    fileDialog.restoreGeometry(settings.value(parent->objectName() % "+Геометрия", "").toByteArray());
    QString newPictureFileName = getPictureFileName(picture) % defaultExportFormat;
    fileDialog.selectFile(newPictureFileName);
    if (fileDialog.exec()) {
        QStringList selectedFiles = fileDialog.selectedFiles();
        if (selectedFiles.length() == 1) {
            QString saveFileFormat;
            QString selectedFile = selectedFiles.at(0);
            if (selectedFile.right(4) == ".jpg") {
                saveFileFormat = "JPEG";
            }
            else if (selectedFile.right(4) == ".jpeg") {
                saveFileFormat = "JPEG";
            }
            else if (selectedFile.right(4) == ".png") {
                saveFileFormat = "PNG";
            }
            else if (selectedFile.right(7) == ".suffix") {
                if (fileDialog.selectedNameFilter() == "Изображение JPEG (*.jpg)") {
                    selectedFile.replace(".suffix", ".jpg");
                }
                else if (fileDialog.selectedNameFilter() == "Переносимая сетевая графика (*.png)") {
                    selectedFile.replace(".suffix", ".png");
                }
                else {
                    selectedFile.replace(".suffix", ".jpg");
                }
            }
            if (QFile::exists(selectedFile)) {
                if (QMessageBox::No == QMessageBox::warning(parent, PictureDialog::tr("Экспортировать как изображение"), PictureDialog::tr("Заменить %1 текущим изображением Snapmatic?").arg("\""+selectedFile+"\""), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)) {
                    goto fileDialogPreSave;
                }
            }
            QImage exportPicture = picture->getImage();
            if (sizeMode == "Desktop") {
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
            else if (sizeMode == "Custom") {
                exportPicture = exportPicture.scaled(cusExportSize, aspectRatio, Qt::SmoothTransformation);
            }
            int errorId = 0;
            bool isSaved = false;
#if QT_VERSION >= 0x050000
            QSaveFile *picFile = new QSaveFile(selectedFile);
#else
            QFile *picFile = new QFile(selectedFile);
#endif
            if (picFile->open(QIODevice::WriteOnly)) {
                isSaved = exportPicture.save(picFile, saveFileFormat.toStdString().c_str(), useCustomQuality ? customQuality : defaultQuality);
#if QT_VERSION >= 0x050000
                if (isSaved) {
                    isSaved = picFile->commit();
                }
                else {
                    errorId = 1;
                }
#else
                picFile->close();
#endif
            }
            else {
                errorId = 2;
            }
            delete picFile;
            if (!isSaved) {
                switch (errorId) {
                case 0:
                    QMessageBox::warning(parent, PictureDialog::tr("Экспортировать как изображение"), PictureDialog::tr("Не удалось экспортировать изображение, поскольку в системе произошла ошибка записи."));
                    break;
                case 1:
                    QMessageBox::warning(parent, PictureDialog::tr("Экспортировать как изображение"), PictureDialog::tr("Не удалось экспортировать изображение, поскольку не удалось определить формат."));
                    break;
                case 2:
                    QMessageBox::warning(parent, PictureDialog::tr("Экспортировать как изображение"), PictureDialog::tr("Не удалось экспортировать изображение, поскольку файл невозможно записать."));
                    break;
                default:
                    QMessageBox::warning(parent, PictureDialog::tr("Экспортировать как изображение"), PictureDialog::tr("Не удалось экспортировать изображение по неизвестной причине."));
                }
                goto fileDialogPreSave;
            }
        }
        else {
            QMessageBox::warning(parent, PictureDialog::tr("Экспортировать как изображение"), PictureDialog::tr("Не выбран действительный файл"));
            goto fileDialogPreSave;
        }
    }
    settings.setValue(parent->objectName() % "+Геометрия", fileDialog.saveGeometry());
    settings.setValue("Directory", fileDialog.directory().absolutePath());
    settings.endGroup();
    settings.endGroup();
}
void PictureExport::exportAsSnapmatic(QWidget *parent, SnapmaticPicture *picture)
{
    QSettings settings(GTA5SYNC_APPVENDOR, GTA5SYNC_APPSTR);
    settings.beginGroup("FileDialogs");
    bool dontUseNativeDialog = settings.value("DontUseNativeDialog", false).toBool();
    settings.beginGroup("ExportAsSnapmatic");
    QString adjustedPicPath = picture->getOriginalPictureFileName();
fileDialogPreSave:
    QFileInfo sgdFileInfo(adjustedPicPath);
    QFileDialog fileDialog(parent);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setViewMode(QFileDialog::Detail);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setOption(QFileDialog::DontUseNativeDialog, dontUseNativeDialog);
    fileDialog.setOption(QFileDialog::DontConfirmOverwrite, true);
    fileDialog.setDefaultSuffix(".rem");
    fileDialog.setWindowFlags(fileDialog.windowFlags()^Qt::WindowContextHelpButtonHint);
    fileDialog.setWindowTitle(PictureDialog::tr("Экспортировать в Snapmatic..."));
    fileDialog.setLabelText(QFileDialog::Accept, PictureDialog::tr("Export"));
    QStringList filters;
    filters << PictureDialog::tr("Экспорт GTA V (*.g5e)");
#ifndef GTA5SYNC_FLATPAK
    filters << PictureDialog::tr("Экспорт GTA V Raw (*.auto)");
#endif
    filters << PictureDialog::tr("Изображения Snapmatic (PGTA*)");
    fileDialog.setNameFilters(filters);
    QList<QUrl> sidebarUrls = SidebarGenerator::generateSidebarUrls(fileDialog.sidebarUrls());
    fileDialog.setSidebarUrls(sidebarUrls);
    fileDialog.setDirectory(settings.value("Directory", StandardPaths::documentsLocation()).toString());
    fileDialog.restoreGeometry(settings.value(parent->objectName() % "+Геометрия", "").toByteArray());
    fileDialog.selectFile(QString(picture->getExportPictureFileName() % ".g5e"));
    if (fileDialog.exec()) {
        QStringList selectedFiles = fileDialog.selectedFiles();
        if (selectedFiles.length() == 1) {
            QString selectedFile = selectedFiles.at(0);
            bool isAutoExt = false;
#ifndef GTA5SYNC_FLATPAK
            if (selectedFile.right(5) == ".auto") {
                isAutoExt = true;
                QString dirPath = QFileInfo(selectedFile).dir().path();
                QString stockFileName = sgdFileInfo.fileName();
                selectedFile = dirPath % "/" % stockFileName;
            }
#endif
            if (selectedFile.right(4) == ".rem") {
                selectedFile.remove(selectedFile.length() - 4, 4);
            }
            if (QFile::exists(selectedFile)) {
                if (QMessageBox::No == QMessageBox::warning(parent, PictureDialog::tr("Экспортировать как Snapmatic"), PictureDialog::tr("Заменить %1 текущим изображением Snapmatic?").arg("\""+selectedFile+"\""), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)) {
                    goto fileDialogPreSave;
                }
            }
            if (selectedFile.right(4) == ".g5e") {
                bool isExported = picture->exportPicture(selectedFile, SnapmaticFormat::G5E_Format);
                if (!isExported) {
                    QMessageBox::warning(parent, PictureDialog::tr("Экспортировать как Snapmatic"), PictureDialog::tr("Не удалось экспортировать текущее изображение Snapmatic."));
                    goto fileDialogPreSave;
                }
            }
            else {
                bool isCopied = picture->exportPicture(selectedFile, SnapmaticFormat::PGTA_Format);
                if (!isCopied) {
                    QMessageBox::warning(parent, PictureDialog::tr("Экспортировать как Snapmatic"), PictureDialog::tr("Не удалось экспортировать текущее изображение Snapmatic."));
                    goto fileDialogPreSave;
                }
                else {
                    if (isAutoExt) QMessageBox::information(parent, PictureDialog::tr("Экспортировать как Snapmatic"), PictureDialog::tr("Snapmatic экспортирован в «%1» из-за использования расширения .auto.").arg(selectedFile));
                }
            }
        }
        else {
            QMessageBox::warning(parent, PictureDialog::tr("Экспортировать как Snapmatic"), PictureDialog::tr("Не выбран действительный файл"));
            goto fileDialogPreSave;
        }
    }
    settings.setValue(parent->objectName() % "+Геометрия", fileDialog.saveGeometry());
    settings.setValue("Directory", fileDialog.directory().absolutePath());
    settings.endGroup();
}
QString PictureExport::getPictureFileName(SnapmaticPicture *picture)
{
    return picture->getExportPictureFileName();
}