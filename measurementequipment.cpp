#include "measurementequipment.h"
#include "ui_measurementequipment.h"

MeasurementEquipment::MeasurementEquipment(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasurementEquipment)
{
    ui->setupUi(this);
}

MeasurementEquipment::~MeasurementEquipment()
{
    delete ui;
}
