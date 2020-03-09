#ifndef CUSTOMKERNEL_H
#define CUSTOMKERNEL_H

#include <QDialog>
#include <QDoubleSpinBox>

#include "mainwindow.h"

namespace Ui {
class customkernel;
}

class customkernel : public QDialog
{
    Q_OBJECT

public:
    customkernel(QWidget *parent = nullptr, int dim_x = 0, int dim_y = 0);
    ~customkernel();

    void get_values();
    int dim_x;
    int dim_y;
    uchar* filter;

    //MainWindow *parentWindow;

    std::vector<std::vector<int> > values;
    int getDivisor();
    int getOffset();

private slots:
    void on_pushButton_clicked();

private:
    Ui::customkernel *ui;
    std::vector<std::vector<QDoubleSpinBox*> > inputs;
    bool compute;
};

#endif // CUSTOMKERNEL_H
