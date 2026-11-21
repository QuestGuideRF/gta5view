#ifndef PLAYERLISTDIALOG_H
#define PLAYERLISTDIALOG_H
#include "ProfileDatabase.h"
#include <QDialog>
namespace Ui {
class PlayerListDialog;
}
class PlayerListDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PlayerListDialog(QStringList players, ProfileDatabase *profileDB, QWidget *parent = 0);
    QStringList getPlayerList() const;
    bool isListUpdated();
    ~PlayerListDialog();
private slots:
    void on_cmdCancel_clicked();
    void on_cmdMakeAv_clicked();
    void on_cmdMakeSe_clicked();
    void on_cmdMakeAd_clicked();
    void on_cmdApply_clicked();
private:
    QStringList players;
    ProfileDatabase *profileDB;
    Ui::PlayerListDialog *ui;
    bool listUpdated;
    void buildInterface();
signals:
    void playerListUpdated(QStringList playerList);
};
#endif