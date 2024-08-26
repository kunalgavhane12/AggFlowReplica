#ifndef SPLITTER_H
#define SPLITTER_H

#include <QDialog>

namespace Ui {
class Splitter;
}

class Splitter : public QDialog
{
    Q_OBJECT

public:
    explicit Splitter(QWidget *parent = nullptr);
    ~Splitter();

private:
    Ui::Splitter *ui;
};

#endif // SPLITTER_H
