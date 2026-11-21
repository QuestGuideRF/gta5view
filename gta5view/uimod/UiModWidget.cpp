#include "UiModWidget.h"
#include <QStyleOption>
#include <QDropEvent>
#include <QMimeData>
#include <QPainter>
#include <QDebug>
#include <QUrl>
UiModWidget::UiModWidget(QWidget *parent) : QWidget(parent)
{
    filesDropEnabled = false;
    imageDropEnabled = false;
}
UiModWidget::~UiModWidget()
{
}
void UiModWidget::setFilesDropEnabled(bool enabled)
{
    filesDropEnabled = enabled;
}
void UiModWidget::setImageDropEnabled(bool enabled)
{
    imageDropEnabled = enabled;
}
void UiModWidget::dragEnterEvent(QDragEnterEvent *dragEnterEvent)
{
    if (filesDropEnabled && dragEnterEvent->mimeData()->hasUrls()) {
        QVector<QString> pathList;
        const QList<QUrl> urlList = dragEnterEvent->mimeData()->urls();
        for (const QUrl &currentUrl : urlList) {
            if (currentUrl.isLocalFile()) {
                pathList.append(currentUrl.toLocalFile());
            }
        }
        if (!pathList.isEmpty()) {
            dragEnterEvent->acceptProposedAction();
        }
    }
    else if (imageDropEnabled && dragEnterEvent->mimeData()->hasImage()) {
        dragEnterEvent->acceptProposedAction();
    }
}
void UiModWidget::dropEvent(QDropEvent *dropEvent)
{
    dropEvent->acceptProposedAction();
    emit dropped(dropEvent->mimeData());
}
void UiModWidget::paintEvent(QPaintEvent *paintEvent)
{
    Q_UNUSED(paintEvent)
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}