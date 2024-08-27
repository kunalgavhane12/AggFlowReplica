#include "endproduct.h"
#include "ui_endproduct.h"

EndProduct::EndProduct(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EndProduct)
{
    ui->setupUi(this);
}

EndProduct::~EndProduct()
{
    delete ui;
}
