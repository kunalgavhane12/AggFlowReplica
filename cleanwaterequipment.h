#ifndef CLEANWATEREQUIPMENT_H
#define CLEANWATEREQUIPMENT_H

#include <QDialog>

namespace Ui {
class CleanWaterEquipment;
}

class CleanWaterEquipment : public QDialog
{
    Q_OBJECT

public:
    explicit CleanWaterEquipment(QWidget *parent = nullptr);
    ~CleanWaterEquipment();

private:
    Ui::CleanWaterEquipment *ui;
};

#endif // CLEANWATEREQUIPMENT_H
