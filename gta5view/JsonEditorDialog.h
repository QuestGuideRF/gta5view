#ifndef JSONEDITORDIALOG_H
#define JSONEDITORDIALOG_H
#include "SnapmaticPicture.h"
#include "JSHighlighter.h"
#include <QDialog>
namespace Ui {
class JsonEditorDialog;
}
class JsonEditorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit JsonEditorDialog(SnapmaticPicture *picture, QWidget *parent = 0);
    bool saveJsonContent();
    ~JsonEditorDialog();
protected:
    void closeEvent(QCloseEvent *ev);
private slots:
    void on_cmdClose_clicked();
    void on_cmdSave_clicked();
signals:
    void codeUpdated(QString jsonCode);
private:
    QString jsonCode;
    JSHighlighter *jsonHl;
    SnapmaticPicture *smpic;
    Ui::JsonEditorDialog *ui;
};
#endif