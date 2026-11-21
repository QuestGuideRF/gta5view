#ifndef SNAPMATICEDITOR_H
#define SNAPMATICEDITOR_H
#include <QDialog>
#include "CrewDatabase.h"
#include "ProfileDatabase.h"
#include "SnapmaticPicture.h"
namespace Ui {
class SnapmaticEditor;
}
class SnapmaticEditor : public QDialog
{
    Q_OBJECT
public:
    explicit SnapmaticEditor(CrewDatabase *crewDB, ProfileDatabase *profileDB, QWidget *parent = 0);
    void setSnapmaticPicture(SnapmaticPicture *picture);
    void setSnapmaticPlayers(const QStringList &players);
    void setSnapmaticTitle(const QString &title);
    void setSnapmaticCrew(const QString &crew = "");
    QString returnCrewName(int crewID);
    ~SnapmaticEditor();
private slots:
    void on_rbSelfie_toggled(bool checked);
    void on_rbMugshot_toggled(bool checked);
    void on_rbEditor_toggled(bool checked);
    void on_rbCustom_toggled(bool checked);
    void on_cmdCancel_clicked();
    void on_cmdApply_clicked();
    void on_cbQualify_toggled(bool checked);
    void on_labPlayers_linkActivated(const QString &link);
    void on_labTitle_linkActivated(const QString &link);
    void on_labCrew_linkActivated(const QString &link);
    void playerListUpdated(QStringList playerList);
private:
    CrewDatabase *crewDB;
    ProfileDatabase *profileDB;
    Ui::SnapmaticEditor *ui;
    SnapmaticProperties snapmaticProperties;
    SnapmaticPicture *smpic;
    QStringList playersList;
    QString snapmaticTitle;
    int crewID;
    bool isSelfie;
    bool isMugshot;
    bool isEditor;
    void selfie_toggled(bool checked);
    void mugshot_toggled(bool checked);
    void editor_toggled(bool checked);
    void qualifyAvatar();
    void insertPlayerNames(QStringList *players);
    QStringList insertPlayerNames(const QStringList &players);
};
#endif