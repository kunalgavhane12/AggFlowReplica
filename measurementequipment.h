#ifndef MEASUREMENTEQUIPMENT_H
#define MEASUREMENTEQUIPMENT_H

#include <QDialog>

namespace Ui {
class MeasurementEquipment;
}

class MeasurementEquipment : public QDialog
{
    Q_OBJECT

public:
    explicit MeasurementEquipment(QWidget *parent = nullptr);
    ~MeasurementEquipment();

private:
    Ui::MeasurementEquipment *ui;
};

#endif // MEASUREMENTEQUIPMENT_H
