#ifndef SCREENINGEQUIPMENT_H
#define SCREENINGEQUIPMENT_H

#include <QDialog>

namespace Ui {
class ScreeningEquipment;
}

class ScreeningEquipment : public QDialog
{
    Q_OBJECT

public:
    explicit ScreeningEquipment(QWidget *parent = nullptr);
    ~ScreeningEquipment();

private:
    Ui::ScreeningEquipment *ui;
};

#endif // SCREENINGEQUIPMENT_H
