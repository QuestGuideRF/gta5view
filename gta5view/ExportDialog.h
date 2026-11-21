#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H
#include <QDialog>
namespace Ui {
class ExportDialog;
}
class ExportDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ExportDialog(QWidget *parent = 0);
    void setupPictureExport();
    bool isSucceeded();
    ~ExportDialog();
private slots:
    void on_cmdSnapmaticClose_clicked();
private:
    Ui::ExportDialog *ui;
    bool success;
};
#endif