#ifndef MAPLOCATIONDIALOG_H
#define MAPLOCATIONDIALOG_H
#include <QDialog>
#include <QMouseEvent>
namespace Ui {
class MapLocationDialog;
}
class MapLocationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MapLocationDialog(double x, double y, QWidget *parent = 0);
    void drawPointOnMap(double x, double y);
    void setCayoPerico(bool isCayoPerico);
    bool propUpdated();
    double getXpos();
    double getYpos();
    ~MapLocationDialog();
protected:
    void paintEvent(QPaintEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);
private slots:
    void on_cmdApply_clicked();
    void on_cmdDone_clicked();
    void on_cmdClose_clicked();
    void on_cmdChange_clicked();
    void on_cmdRevert_clicked();
    void updatePosFromEvent(double x, double y);
private:
    int zoomPercent;
    double xpos_old;
    double ypos_old;
    double xpos_new;
    double ypos_new;
    bool dragStart;
    bool propUpdate;
    bool changeMode;
    bool p_isCayoPerico;
    QImage mapImage;
    QPointF dragPosition;
    QPointF mapDiffPosition;
    Ui::MapLocationDialog *ui;
};
#endif