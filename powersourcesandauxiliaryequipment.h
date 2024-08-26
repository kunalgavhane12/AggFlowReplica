#ifndef POWERSOURCESANDAUXILIARYEQUIPMENT_H
#define POWERSOURCESANDAUXILIARYEQUIPMENT_H

#include <QDialog>

namespace Ui {
class PowerSourcesandAuxiliaryEquipment;
}

class PowerSourcesandAuxiliaryEquipment : public QDialog
{
    Q_OBJECT

public:
    explicit PowerSourcesandAuxiliaryEquipment(QWidget *parent = nullptr);
    ~PowerSourcesandAuxiliaryEquipment();

private:
    Ui::PowerSourcesandAuxiliaryEquipment *ui;
};

#endif // POWERSOURCESANDAUXILIARYEQUIPMENT_H
