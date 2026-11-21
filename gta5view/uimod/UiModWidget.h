#ifndef UIMODWIDGET_H
#define UIMODWIDGET_H
#include <QMimeData>
#include <QWidget>
#include <QString>
#include <QSize>
class UiModWidget : public QWidget
{
    Q_OBJECT
public:
    UiModWidget(QWidget *parent = 0);
    void setFilesDropEnabled(bool enabled);
    void setImageDropEnabled(bool enabled);
    ~UiModWidget();
protected:
    void dragEnterEvent(QDragEnterEvent *dragEnterEvent);
    void dropEvent(QDropEvent *dropEvent);
    void paintEvent(QPaintEvent *paintEvent);
private:
    bool filesDropEnabled;
    bool imageDropEnabled;
signals:
    void dropped(const QMimeData *mimeData);
};
#endif