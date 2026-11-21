#ifndef PICTUREWIDGET_H
#define PICTUREWIDGET_H
#include "UiModLabel.h"
#include <QHBoxLayout>
#include <QDialog>
#include <QWidget>
#include <QEvent>
class PictureWidget : public QDialog
{
    Q_OBJECT
public:
    explicit PictureWidget(QWidget *parent = 0);
    void setImage(QImage image, QRect rec);
    ~PictureWidget();
public slots:
    void setImage(QImage image);
protected:
    bool eventFilter(QObject *obj, QEvent *ev);
private:
    QHBoxLayout *widgetLayout;
    UiModLabel *pictureLabel;
    QImage image;
private slots:
    void pictureDoubleClicked(Qt::MouseButton button);
signals:
    void nextPictureRequested();
    void previousPictureRequested();
};
#endif