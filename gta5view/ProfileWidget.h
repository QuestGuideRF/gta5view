#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H
#include <QWidget>
class ProfileWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ProfileWidget(QWidget *parent = 0);
    virtual void setSelectionMode(bool selectionMode);
    virtual void setContentMode(int contentMode);
    virtual void setSelected(bool isSelected);
    virtual bool isSelected();
    virtual QString getWidgetType();
    virtual int getContentMode();
    virtual void retranslate();
    ~ProfileWidget();
private:
    int contentMode;
signals:
public slots:
};
#endif