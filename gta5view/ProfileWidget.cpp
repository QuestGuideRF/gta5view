#include "ProfileWidget.h"
#include <QDebug>
ProfileWidget::ProfileWidget(QWidget *parent) : QWidget(parent)
{
    contentMode = 0;
}
ProfileWidget::~ProfileWidget()
{
}
void ProfileWidget::retranslate()
{
    qDebug() << "ProfileWidget::retranslate используется без перезаписи";
}
bool ProfileWidget::isSelected()
{
    qDebug() << "ProfileWidget::isSelected используется без перезаписи";
    return false;
}
void ProfileWidget::setSelected(bool isSelected)
{
    qDebug() << "ProfileWidget::setSelected использовался без перезаписи, результат" << isSelected;
}
void ProfileWidget::setSelectionMode(bool selectionMode)
{
    qDebug() << "ProfileWidget::setSelectionMode используется без перезаписи, результат:" << selectionMode;
}
QString ProfileWidget::getWidgetType()
{
    qDebug() << "ProfileWidget::getWidgetType используется без перезаписи";
    return "ProfileWidget";
}
int ProfileWidget::getContentMode()
{
    return contentMode;
}
void ProfileWidget::setContentMode(int _contentMode)
{
    contentMode = _contentMode;
}