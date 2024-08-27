#include "screeningequipment.h"
#include "ui_screeningequipment.h"

ScreeningEquipment::ScreeningEquipment(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScreeningEquipment)
{
    ui->setupUi(this);
}

ScreeningEquipment::~ScreeningEquipment()
{
    delete ui;
}
