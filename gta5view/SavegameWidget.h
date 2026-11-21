#ifndef SAVEGAMEWIDGET_H
#define SAVEGAMEWIDGET_H
#include "ProfileWidget.h"
#include "SavegameData.h"
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QWidget>
#include <QColor>
namespace Ui {
class SavegameWidget;
}
class SavegameWidget : public ProfileWidget
{
    Q_OBJECT
public:
    SavegameWidget(QWidget *parent = 0);
    void setSavegameData(SavegameData *savegame, QString savegamePath);
    void setSelectionMode(bool selectionMode);
    void setSelected(bool isSelected);
    SavegameData* getSavegame();
    QString getWidgetType();
    bool isSelected();
    void retranslate();
    ~SavegameWidget();
private slots:
    void on_cmdView_clicked();
    void on_cmdCopy_clicked();
    void on_cmdDelete_clicked();
    void on_cbSelected_stateChanged(int arg1);
    void savegameSelected();
    void selectAllWidgets();
    void deselectAllWidgets();
protected:
    void mouseDoubleClickEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void contextMenuEvent(QContextMenuEvent *ev);
private:
    Ui::SavegameWidget *ui;
    SavegameData *sgdata;
    QString labelAutosaveStr;
    QString labelSaveStr;
    QString sgdPath;
    QString sgdStr;
    void renderString(const QString &savegameString, const QString &fileName);
signals:
    void savegameDeleted();
    void widgetSelected();
    void widgetDeselected();
    void allWidgetsSelected();
    void allWidgetsDeselected();
    void contextMenuTriggered(QContextMenuEvent *ev);
};
#endif