#include "ExportDialog.h"
#include "ui_ExportDialog.h"
ExportDialog::ExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);
    success = false;
}
ExportDialog::~ExportDialog()
{
    delete ui;
}
bool ExportDialog::isSucceeded()
{
    return success;
}
void ExportDialog::on_cmdSnapmaticClose_clicked()
{
    this->close();
}
void ExportDialog::setupPictureExport()
{
    ui->swExport->setCurrentWidget(ui->pageSnapmatic);
}