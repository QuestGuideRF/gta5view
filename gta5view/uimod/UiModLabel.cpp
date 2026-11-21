#include "UiModLabel.h"
#include <QPaintEvent>
#include <QMouseEvent>
UiModLabel::UiModLabel(const QString &text, QWidget *parent) : QLabel(parent)
{
    setText(text);
}
UiModLabel::UiModLabel(QWidget *parent, const QString &text) : QLabel(parent)
{
    setText(text);
}
UiModLabel::UiModLabel(QWidget *parent) : QLabel(parent)
{
}
UiModLabel::~UiModLabel()
{
}
void UiModLabel::paintEvent(QPaintEvent *ev)
{
    QLabel::paintEvent(ev);
    emit labelPainted();
}
void UiModLabel::mouseMoveEvent(QMouseEvent *ev)
{
    QLabel::mouseMoveEvent(ev);
    emit mouseMoved();
}
void UiModLabel::mousePressEvent(QMouseEvent *ev)
{
    QLabel::mousePressEvent(ev);
    emit mousePressed(ev->button());
}
void UiModLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    QLabel::mouseReleaseEvent(ev);
    emit mouseReleased(ev->button());
}
void UiModLabel::mouseDoubleClickEvent(QMouseEvent *ev)
{
    QLabel::mouseDoubleClickEvent(ev);
    emit mouseDoubleClicked(ev->button());
}
void UiModLabel::resizeEvent(QResizeEvent *ev)
{
    QLabel::resizeEvent(ev);
    emit resized(ev->size());
}