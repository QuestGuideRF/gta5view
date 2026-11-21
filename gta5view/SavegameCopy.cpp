#include "SidebarGenerator.h"
#include "SavegameWidget.h"
#include "StandardPaths.h"
#include "SavegameCopy.h"
#include "config.h"
#include <QStringBuilder>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
SavegameCopy::SavegameCopy()
{
}
void SavegameCopy::copySavegame(QWidget *parent, QString sgdPath)
{
    QSettings settings(GTA5SYNC_APPVENDOR, GTA5SYNC_APPSTR);
    settings.beginGroup("FileDialogs");
    bool dontUseNativeDialog = settings.value("DontUseNativeDialog", false).toBool();
    settings.beginGroup("SavegameCopy");
fileDialogPreSave:
    QFileInfo sgdFileInfo(sgdPath);
    QFileDialog fileDialog(parent);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setViewMode(QFileDialog::Detail);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setOption(QFileDialog::DontUseNativeDialog, dontUseNativeDialog);
    fileDialog.setOption(QFileDialog::DontConfirmOverwrite, true);
    fileDialog.setDefaultSuffix("");
    fileDialog.setWindowFlags(fileDialog.windowFlags()^Qt::WindowContextHelpButtonHint);
    fileDialog.setWindowTitle(SavegameWidget::tr(("Экспортировать сохранение...")));
    fileDialog.setLabelText(QFileDialog::Accept, SavegameWidget::tr("Export"));
    QStringList filters;
    filters << SavegameWidget::tr("Файлы сохранений (SGTA*)");
    filters << SavegameWidget::tr("Все файлы (**)");
    fileDialog.setNameFilters(filters);
    QList<QUrl> sidebarUrls = SidebarGenerator::generateSidebarUrls(fileDialog.sidebarUrls());
    fileDialog.setSidebarUrls(sidebarUrls);
    fileDialog.setDirectory(settings.value("Directory", StandardPaths::picturesLocation()).toString());
    fileDialog.restoreGeometry(settings.value(parent->objectName() % "+Геометрия", "").toByteArray());
    fileDialog.selectFile(sgdFileInfo.fileName());
    if (fileDialog.exec())
    {
        QStringList selectedFiles = fileDialog.selectedFiles();
        if (selectedFiles.length() == 1)
        {
            QString selectedFile = selectedFiles.at(0);
            if (QFile::exists(selectedFile))
            {
                if (QMessageBox::Yes == QMessageBox::warning(parent, SavegameWidget::tr("Экспортировать сохранение"), SavegameWidget::tr("Перезаписать %1 текущим сохранением?").arg("\""+selectedFile+"\""), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
                {
                    if (!QFile::remove(selectedFile))
                    {
                        QMessageBox::warning(parent, SavegameWidget::tr("Экспортировать сохранение"), SavegameWidget::tr("Не удалось перезаписать %1 текущим сохранением.").arg("\""+selectedFile+"\""));
                        goto fileDialogPreSave;
                    }
                }
                else
                {
                    goto fileDialogPreSave;
                }
            }
            bool isCopied = QFile::copy(sgdPath, selectedFile);
            if (!isCopied)
            {
                QMessageBox::warning(parent, SavegameWidget::tr("Экспортировать сохранение"), SavegameWidget::tr("Не удалось экспортировать текущее сохранение."));
                goto fileDialogPreSave;
            }
        }
        else
        {
            QMessageBox::warning(parent, SavegameWidget::tr("Экспортировать сохранение"), SavegameWidget::tr("Не выбран действительный файл"));
            goto fileDialogPreSave;
        }
    }
    settings.setValue(parent->objectName() % "+Геометрия", fileDialog.saveGeometry());
    settings.setValue("Directory", fileDialog.directory().absolutePath());
    settings.endGroup();
    settings.endGroup();
}