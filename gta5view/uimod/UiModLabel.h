#ifndef UIMODLABEL_H
#define UIMODLABEL_H
#include <QWidget>
#include <QString>
#include <QLabel>
#include <QSize>
class UiModLabel : public QLabel
{
    Q_OBJECT
public:
    UiModLabel(const QString &text, QWidget *parent = 0);
    UiModLabel(QWidget *parent, const QString &text);
    UiModLabel(QWidget *parent = 0);
    ~UiModLabel();
protected:
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseDoubleClickEvent(QMouseEvent *ev);
    void paintEvent(QPaintEvent *ev);
    void resizeEvent(QResizeEvent *ev);
signals:
    void mouseMoved();
    void mousePressed(Qt::MouseButton button);
    void mouseReleased(Qt::MouseButton button);
    void mouseDoubleClicked(Qt::MouseButton button);
    void labelPainted();
    void resized(QSize newSize);
};
#endif