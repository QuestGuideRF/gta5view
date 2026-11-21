#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H
#include <QSize>
#include <QList>
#include <QDialog>
#include <QSettings>
#include <QTreeWidgetItem>
#include "ProfileDatabase.h"
namespace Ui {
class OptionsDialog;
}
class OptionsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit OptionsDialog(ProfileDatabase *profileDB, QWidget *parent = 0);
    void commitProfiles(const QStringList &profiles);
    ~OptionsDialog();
private slots:
    void on_cmdOK_clicked();
    void on_rbPicCustomRes_toggled(bool checked);
    void on_cbPicCustomQuality_toggled(bool checked);
    void on_hsPicQuality_valueChanged(int value);
    void on_cbIgnoreAspectRatio_toggled(bool checked);
    void on_cmdExploreFolder_clicked();
    void on_cbDefaultStyle_toggled(bool checked);
    void on_cbDefaultFont_toggled(bool checked);
    void on_cmdCopyStatsID_clicked();
    void on_cbFont_currentFontChanged(const QFont &font);
    void on_cmdFont_clicked();
signals:
    void settingsApplied(int contentMode, bool languageChanged);
private:
    ProfileDatabase *profileDB;
    Ui::OptionsDialog *ui;
    QList<QTreeWidgetItem*> playerItems;
    Qt::AspectRatioMode aspectRatio;
    QString currentAreaLanguage;
    QString currentLanguage;
    QString currentCFolder;
    QString defaultProfile;
    QString percentString;
    QSettings *settings;
    bool withoutPlayers;
    bool currentFFolder;
    int contentMode;
    int customQuality;
    int defaultQuality;
    QSize defExportSize;
    QSize cusExportSize;
    void setupTreeWidget();
    void setupLanguageBox();
    void setupRadioButtons();
    void setupDefaultProfile();
    void setupPictureSettings();
    void setupCustomGTAFolder();
    void setupInterfaceSettings();
    void setupStatisticsSettings();
    void setupSnapmaticPictureViewer();
    void setupWindowsGameSettings();
    void applySettings();
};
#endif