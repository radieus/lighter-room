#include "customkernel.h"
#include "ui_customkernel.h"
#include <QDebug>

customkernel::customkernel(QWidget *parent, int _dim_x, int _dim_y)
    : QDialog(parent), dim_x(_dim_x), dim_y(_dim_y), ui(new Ui::customkernel)
{
    ui->setupUi(this);

    //this->parentWindow = window;

    for(int i = 0; i < dim_x; i++) {

        std::vector<QDoubleSpinBox*> row;
        inputs.push_back(row);

        for(int j = 0; j < dim_y; j++)
        {
            inputs[i].push_back(new QDoubleSpinBox(this));
            inputs[i][j]->setRange(-1000, 1000);
            ui->gridLayout->addWidget(inputs[i][j], i, j);
        }
    }

}

customkernel::~customkernel()
{
    delete ui;
}

void customkernel::get_values() {
    std::vector<std::vector<int> > values;
    for (int i = 0; i < dim_x; i++) {
        std::vector<int> row;
        values.push_back(row);

        for (int j = 0; j < dim_y; j++) {
            values[i].push_back(inputs[i][j]->value());
            //qDebug() << inputs[i][j]->value();
        }
    }

    this->values = values;

}
int customkernel::getDivisor() {
    return this->ui->lineEditFilterDivisor->text().toInt();
}

int customkernel::getOffset() {
    return this->ui->lineEditFilterOffset->text().toInt();
}

void customkernel::on_pushButton_clicked()
{
    this->get_values();
    this->accept();
}


