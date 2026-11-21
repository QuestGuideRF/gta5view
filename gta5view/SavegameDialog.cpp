#include "SavegameDialog.h"
#include "ui_SavegameDialog.h"
#include "SavegameCopy.h"
#include "AppEnv.h"
#include <QMessageBox>
#include <QDebug>
SavegameDialog::SavegameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SavegameDialog)
{
#if QT_VERSION >= 0x050900
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
#else
    setWindowFlags(windowFlags()^Qt::WindowContextHelpButtonHint);
#endif
    ui->setupUi(this);
    ui->cmdClose->setFocus();
    savegameLabStr = ui->labSavegameText->text();
    if (QIcon::hasThemeIcon("dialog-close"))
    {
        ui->cmdClose->setIcon(QIcon::fromTheme("dialog-close"));
    }
    else if (QIcon::hasThemeIcon("gtk-close"))
    {
        ui->cmdClose->setIcon(QIcon::fromTheme("gtk-close"));
    }
    if (QIcon::hasThemeIcon("document-export"))
    {
        ui->cmdCopy->setIcon(QIcon::fromTheme("document-export"));
    }
    else if (QIcon::hasThemeIcon("document-save"))
    {
        ui->cmdCopy->setIcon(QIcon::fromTheme("document-save"));
    }
    refreshWindowSize();
}
SavegameDialog::~SavegameDialog()
{
    delete ui;
}
void SavegameDialog::refreshWindowSize()
{
    qreal screenRatio = AppEnv::screenRatio();
    int dpiWindowWidth = 400 * screenRatio;
    int dpiWindowHeight = 105 * screenRatio;
    if (dpiWindowHeight < heightForWidth(dpiWindowWidth))
    {
        dpiWindowHeight = heightForWidth(dpiWindowWidth);
    }
    resize(dpiWindowWidth, dpiWindowHeight);
}
void SavegameDialog::setSavegameData(SavegameData *savegame, QString savegamePath, bool readOk)
{
    if (!readOk)
    {
        QMessageBox::warning(this,tr("Просмотрщик сохранений"),tr("Ошибка на %1").arg(savegame->getLastStep()));
        return;
    }
    sgdPath = savegamePath;
    ui->labSavegameText->setText(savegameLabStr.arg(savegame->getSavegameStr()));
    refreshWindowSize();
}
void SavegameDialog::on_cmdClose_clicked()
{
    this->close();
}
void SavegameDialog::on_cmdCopy_clicked()
{
    SavegameCopy::copySavegame(this, sgdPath);
}