#include "cleanwaterequipment.h"
#include "ui_cleanwaterequipment.h"

CleanWaterEquipment::CleanWaterEquipment(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CleanWaterEquipment)
{
    ui->setupUi(this);
}

CleanWaterEquipment::~CleanWaterEquipment()
{
    delete ui;
}
