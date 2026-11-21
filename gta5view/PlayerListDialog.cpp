#include "PlayerListDialog.h"
#include "ui_PlayerListDialog.h"
#include "wrapper.h"
#include "AppEnv.h"
#include <QStringBuilder>
#include <QFontMetrics>
#include <QInputDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
PlayerListDialog::PlayerListDialog(QStringList players, ProfileDatabase *profileDB, QWidget *parent) :
    QDialog(parent), players(players), profileDB(profileDB),
    ui(new Ui::PlayerListDialog)
{
#if QT_VERSION >= 0x050900
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
#else
    setWindowFlags(windowFlags()^Qt::WindowContextHelpButtonHint);
#endif
    listUpdated = false;
    ui->setupUi(this);
    ui->cmdCancel->setDefault(true);
    ui->cmdCancel->setFocus();
    if (QIcon::hasThemeIcon("dialog-ok-apply")) {
        ui->cmdApply->setIcon(QIcon::fromTheme("dialog-ok-apply"));
    }
    else if (QIcon::hasThemeIcon("dialog-apply")) {
        ui->cmdApply->setIcon(QIcon::fromTheme("dialog-apply"));
    }
    else if (QIcon::hasThemeIcon("gtk-apply")) {
        ui->cmdApply->setIcon(QIcon::fromTheme("gtk-apply"));
    }
    else if (QIcon::hasThemeIcon("dialog-ok")) {
        ui->cmdApply->setIcon(QIcon::fromTheme("dialog-ok"));
    }
    else if (QIcon::hasThemeIcon("gtk-ok")) {
        ui->cmdApply->setIcon(QIcon::fromTheme("dialog-ok"));
    }
    if (QIcon::hasThemeIcon("dialog-cancel")) {
        ui->cmdCancel->setIcon(QIcon::fromTheme("dialog-cancel"));
    }
    else if (QIcon::hasThemeIcon("gtk-cancel")) {
        ui->cmdCancel->setIcon(QIcon::fromTheme("gtk-cancel"));
    }
    if (QIcon::hasThemeIcon("go-previous") && QIcon::hasThemeIcon("go-next") && QIcon::hasThemeIcon("list-add")) {
#if QT_VERSION < 0x050600
        qreal screenRatio = AppEnv::screenRatio();
        if (screenRatio != 1) {
            QSize iconSize = ui->cmdMakeAv->iconSize();
            iconSize = QSize(iconSize.width() * screenRatio, iconSize.height() * screenRatio);
            ui->cmdMakeAv->setIconSize(iconSize);
            ui->cmdMakeSe->setIconSize(iconSize);
            ui->cmdMakeAd->setIconSize(iconSize);
        }
#endif
        ui->cmdMakeAv->setIcon(QIcon::fromTheme("go-previous"));
        ui->cmdMakeSe->setIcon(QIcon::fromTheme("go-next"));
        ui->cmdMakeAd->setIcon(QIcon::fromTheme("list-add"));
    }
    else {
#if QT_VERSION < 0x050600
        qreal screenRatio = AppEnv::screenRatio();
        if (screenRatio != 1) {
            QSize iconSize = ui->cmdMakeAv->iconSize();
            iconSize = QSize(iconSize.width() * screenRatio, iconSize.height() * screenRatio);
            ui->cmdMakeAv->setIconSize(iconSize);
            ui->cmdMakeSe->setIconSize(iconSize);
            ui->cmdMakeAd->setIconSize(iconSize);
        }
#endif
        ui->cmdMakeAv->setIcon(QIcon(AppEnv::getImagesFolder() % "/back.svgz"));
        ui->cmdMakeSe->setIcon(QIcon(AppEnv::getImagesFolder() % "/next.svgz"));
        ui->cmdMakeAd->setIcon(QIcon(AppEnv::getImagesFolder() % "/add.svgz"));
    }
    buildInterface();
    qreal screenRatio = AppEnv::screenRatio();
    resize(500 * screenRatio, 350 * screenRatio);
}
PlayerListDialog::~PlayerListDialog()
{
    for (QObject *object : ui->listAvPlayers->children()) {
        delete object;
    }
    for (QObject *object : ui->listSePlayers->children()) {
        delete object;
    }
    delete ui;
}
void PlayerListDialog::on_cmdCancel_clicked()
{
    close();
}
void PlayerListDialog::buildInterface()
{
    const QStringList dbPlayers = profileDB->getPlayers();
    for (const QString &sePlayer : qAsConst(players)) {
        QListWidgetItem *playerItem = new QListWidgetItem(profileDB->getPlayerName(sePlayer));
        playerItem->setData(Qt::UserRole, sePlayer);
        ui->listSePlayers->addItem(playerItem);
    }
    for (const QString &dbPlayer : dbPlayers) {
        if (!players.contains(dbPlayer)) {
            QListWidgetItem *playerItem = new QListWidgetItem(profileDB->getPlayerName(dbPlayer));
            playerItem->setData(Qt::UserRole, dbPlayer);
            ui->listAvPlayers->addItem(playerItem);
        }
    }
    ui->listAvPlayers->sortItems(Qt::AscendingOrder);
}
void PlayerListDialog::on_cmdMakeAv_clicked()
{
    for (QListWidgetItem *item : ui->listSePlayers->selectedItems()) {
        QString playerName = item->text();
        int playerID = item->data(Qt::UserRole).toInt();
        delete item;
        QListWidgetItem *playerItem = new QListWidgetItem(playerName);
        playerItem->setData(Qt::UserRole, playerID);
        ui->listAvPlayers->addItem(playerItem);
        ui->listAvPlayers->sortItems(Qt::AscendingOrder);
    }
}
void PlayerListDialog::on_cmdMakeSe_clicked()
{
    int maxPlayers = 30;
    if (maxPlayers < ui->listSePlayers->count() + ui->listAvPlayers->selectedItems().count()) {
        QMessageBox::warning(this, tr("Добавить игроков..."), tr("Не удалось добавить больше игроков, поскольку максимальное количество игроков составляет %1!").arg(QString::number(maxPlayers)));
        return;
    }
    for (QListWidgetItem *item : ui->listAvPlayers->selectedItems()) {
        QString playerName = item->text();
        int playerID = item->data(Qt::UserRole).toInt();
        delete item;
        QListWidgetItem *playerItem = new QListWidgetItem(playerName);
        playerItem->setData(Qt::UserRole, playerID);
        ui->listSePlayers->addItem(playerItem);
    }
}
void PlayerListDialog::on_cmdMakeAd_clicked()
{
    bool playerOk;
    int playerID = QInputDialog::getInt(this, tr("Добавить игрока..."), tr("Введите идентификатор игрока Social Club"), 1, 1, 214783647, 1, &playerOk, windowFlags());
    if (playerOk) {
        for (int i = 0; i < ui->listAvPlayers->count(); ++i) {
            QListWidgetItem *item = ui->listAvPlayers->item(i);
            QString itemPlayerName = item->text();
            int itemPlayerID = item->data(Qt::UserRole).toInt();
            if (itemPlayerID == playerID) {
                delete item;
                QListWidgetItem *playerItem = new QListWidgetItem(itemPlayerName);
                playerItem->setData(Qt::UserRole, playerID);
                ui->listSePlayers->addItem(playerItem);
                return;
            }
        }
        for (int i = 0; i < ui->listSePlayers->count(); ++i) {
            QListWidgetItem *item = ui->listSePlayers->item(i);
            int itemPlayerID = item->data(Qt::UserRole).toInt();
            if (itemPlayerID == playerID)
            {
                QMessageBox::warning(this, tr("Добавить игрока..."), tr("Не удалось добавить игрока %1, поскольку игрок %1 уже добавлен!").arg(QString::number(playerID)));
                return;
            }
        }
        QListWidgetItem *playerItem = new QListWidgetItem(QString::number(playerID));
        playerItem->setData(Qt::UserRole, playerID);
        ui->listSePlayers->addItem(playerItem);
    }
}
void PlayerListDialog::on_cmdApply_clicked()
{
    players.clear();
    for (int i = 0; i < ui->listSePlayers->count(); ++i) {
        players += ui->listSePlayers->item(i)->data(Qt::UserRole).toString();
    }
    emit playerListUpdated(players);
    listUpdated = true;
    close();
}
QStringList PlayerListDialog::getPlayerList() const
{
    return players;
}
bool PlayerListDialog::isListUpdated()
{
    return listUpdated;
}