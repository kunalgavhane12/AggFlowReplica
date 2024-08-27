#ifndef ENDPRODUCT_H
#define ENDPRODUCT_H

#include <QDialog>

namespace Ui {
class EndProduct;
}

class EndProduct : public QDialog
{
    Q_OBJECT

public:
    explicit EndProduct(QWidget *parent = nullptr);
    ~EndProduct();

private:
    Ui::EndProduct *ui;
};

#endif // ENDPRODUCT_H
