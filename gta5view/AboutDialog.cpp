#include <QDesktopServices>
#include <QStringBuilder>
#include <QMessageBox>
#include "AboutDialog.h"
#include "ui_AboutDialog.h"
#include "AppEnv.h"
#include "config.h"
AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
#if QT_VERSION >= 0x050900
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
#else
    setWindowFlags(windowFlags()^Qt::WindowContextHelpButtonHint);
#endif
    QString appVersion = QApplication::applicationVersion();
    const char* literalBuildType = GTA5SYNC_BUILDTYPE;
    const QString buildType = tr(literalBuildType);
    const QString projectBuild = AppEnv::getBuildDateTime();
    const QString buildStr = GTA5SYNC_BUILDSTRING;
#ifdef GTA5SYNC_COMMIT
    if ((strcmp(literalBuildType, REL_BUILDTYPE) != 0) && !appVersion.contains("-"))
        appVersion = appVersion % "-" % GTA5SYNC_COMMIT;
#endif
    const QString translatedByStr = tr("Перевел %1");
    const QString translatorVal = tr("TRANSLATOR");
    QStringList translatorContent;
    if (translatorVal != "TRANSLATOR") {
        const QStringList translatorList = translatorVal.split('\n');
        for (const QString &translatorStr : translatorList) {
            QStringList translatorStrList = translatorStr.split(',');
            const QString translatorName = translatorStrList.at(0);
            translatorStrList.removeFirst();
            QString translatorProfile = translatorStrList.join(QString());
            if (!translatorProfile.isEmpty()) {
                translatorContent += QString("<a href=\"%1\">%2</a>").arg(translatorProfile, translatorName);
            }
            else {
                translatorContent += translatorName;
            }
        }
    }
    const QString projectDes = tr("Проект для просмотра Grand Theft Auto V Snapmatic<br/>\nИзображения и сохранения");
    QString copyrightDes1 = tr("Copyright &copy; <a href=\"%1\">%2</a> %3");
    copyrightDes1 = copyrightDes1.arg(GTA5SYNC_APPVENDORLINK, GTA5SYNC_APPVENDOR, GTA5SYNC_COPYRIGHT);
    QString copyrightDes2 = tr("%1 is licensed under <a href=\"https://www.gnu.org/licenses/gpl-3.0.html#content\">GNU GPLv3</a>");
    copyrightDes2 = copyrightDes2.arg(GTA5SYNC_APPSTR);
    QString copyrightDesA;
    if (!translatorContent.isEmpty()) {
        copyrightDesA = copyrightDes1 % "<br/>" % translatedByStr.arg(translatorContent.join(", ")) % "<br/>" % copyrightDes2;
    }
    else {
        copyrightDesA = copyrightDes1 % "<br/>" % copyrightDes2;
    }
    ui->setupUi(this);
    aboutStr = ui->labAbout->text();
    titleStr = windowTitle();
    ui->labAbout->setText(aboutStr.arg(GTA5SYNC_APPSTR, projectDes, appVersion % " (" % buildType % ")", projectBuild, buildStr, qVersion(), copyrightDesA));
    setWindowTitle(titleStr.arg(GTA5SYNC_APPSTR));
    if (QIcon::hasThemeIcon("dialog-close")) {
        ui->cmdClose->setIcon(QIcon::fromTheme("dialog-close"));
    }
    else if (QIcon::hasThemeIcon("gtk-close")) {
        ui->cmdClose->setIcon(QIcon::fromTheme("gtk-close"));
    }
    qreal screenRatio = AppEnv::screenRatio();
    if (!translatorContent.isEmpty()) {
        resize(375 * screenRatio, 270 * screenRatio);
    }
    else {
        resize(375 * screenRatio, 260 * screenRatio);
    }
}
AboutDialog::~AboutDialog()
{
    delete ui;
}
void AboutDialog::on_labAbout_linkActivated(const QString &link)
{
    if (link.left(12) == "g5e://о?") {
        QStringList aboutStrList = QString(link).remove(0, 12).split(":");
        QMessageBox::information(this, QString::fromUtf8(QByteArray::fromBase64(aboutStrList.at(0).toUtf8())), QString::fromUtf8(QByteArray::fromBase64(aboutStrList.at(1).toUtf8())));
    }
    else {
        QDesktopServices::openUrl(QUrl(link));
    }
}