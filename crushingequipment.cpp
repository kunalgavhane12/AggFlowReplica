#include "crushingequipment.h"
#include "ui_crushingequipment.h"

CrushingEquipment::CrushingEquipment(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrushingEquipment)
{
    ui->setupUi(this);
}

CrushingEquipment::~CrushingEquipment()
{
    delete ui;
}
