#ifndef CRUSHINGEQUIPMENT_H
#define CRUSHINGEQUIPMENT_H

#include <QDialog>

namespace Ui {
class CrushingEquipment;
}

class CrushingEquipment : public QDialog
{
    Q_OBJECT

public:
    explicit CrushingEquipment(QWidget *parent = nullptr);
    ~CrushingEquipment();

private:
    Ui::CrushingEquipment *ui;
};

#endif // CRUSHINGEQUIPMENT_H
