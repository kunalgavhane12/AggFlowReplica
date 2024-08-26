#include "powersourcesandauxiliaryequipment.h"
#include "ui_powersourcesandauxiliaryequipment.h"

PowerSourcesandAuxiliaryEquipment::PowerSourcesandAuxiliaryEquipment(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PowerSourcesandAuxiliaryEquipment)
{
    ui->setupUi(this);
}

PowerSourcesandAuxiliaryEquipment::~PowerSourcesandAuxiliaryEquipment()
{
    delete ui;
}
