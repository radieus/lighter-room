#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSave_as_triggered()
{
    QString _filename = QFileDialog::getSaveFileName(this, tr("Save Image File"), QString(), tr("Images (*.jpg)"));
    if (!_filename.isEmpty()){
      current.save(_filename);
    }
}

void MainWindow::on_actionOpen_triggered()
{
    filename = QFileDialog::getOpenFileName(this, tr("Choose"), "", tr("Images (*.png *.PNG *.jpg *.jpeg *.bmp"));

    if(QString::compare(filename, QString()) != 0){

        bool valid = originalImage.load(filename);
        finalImage.load(filename);

        if(valid){
            current = QPixmap::fromImage(originalImage);
            QRect rect(10, 10, current.width()-10, current.height()-10);
            ui->originalLabel->setPixmap(QPixmap::fromImage(originalImage).copy(rect));
            ui->finalLabel->setPixmap(QPixmap::fromImage(finalImage).copy(rect));
        }
        else {
            //handle
        }
    }
}

void MainWindow::on_actionRestore_triggered()
{
    current = QPixmap::fromImage(originalImage);
    ui->finalLabel->setPixmap(QPixmap::fromImage(originalImage).scaled(ui->finalLabel->width(), ui->finalLabel->height(), Qt::KeepAspectRatio));
}

void MainWindow::on_actionInvert_triggered()
{
    QImage image = current.toImage().convertToFormat(QImage::Format_RGB888);
    uchar* pix = image.bits();
    uchar* end_pix = pix + image.sizeInBytes();

    for (; pix < end_pix; pix++){
        *pix = 255 - *pix;
    }

    current = QPixmap::fromImage(image);
    ui->finalLabel->setPixmap(current.scaled(ui->finalLabel->width(), ui->finalLabel->height(), Qt::KeepAspectRatio));
}

void MainWindow::on_actionBrighten_triggered()
{
    QImage image = current.toImage().convertToFormat(QImage::Format_RGB888);
    uchar* pix = image.bits();
    uchar* end_pix = pix + image.sizeInBytes();

    for (; pix < end_pix; pix++){
        *pix = qBound(0.0, *pix + 20.0, 255.0);
    }

    current = QPixmap::fromImage(image);
    ui->finalLabel->setPixmap(current.scaled(ui->finalLabel->width(), ui->finalLabel->height(), Qt::KeepAspectRatio));
}

void MainWindow::on_actionDarken_triggered()
{
    QImage image = current.toImage().convertToFormat(QImage::Format_RGB888);
    uchar* pix = image.bits();
    uchar* end_pix = pix + image.sizeInBytes();

    for (; pix < end_pix; pix++){
        *pix = qBound(0.0, *pix - 20.0, 255.0);
    }

    current = QPixmap::fromImage(image);
    ui->finalLabel->setPixmap(current.scaled(ui->finalLabel->width(), ui->finalLabel->height(), Qt::KeepAspectRatio));
}

void MainWindow::on_actionEnchance_contrast_triggered()
{
    QImage image = current.toImage().convertToFormat(QImage::Format_RGB888);
    uchar* pix = image.bits();
    uchar* end_pix = pix + image.sizeInBytes();

    for (; pix < end_pix; pix++){
        *pix = qBound(0.0, 1.1*(*pix - 128) + 128, 255.0);
    }

    current = QPixmap::fromImage(image);
    ui->finalLabel->setPixmap(current.scaled(ui->finalLabel->width(), ui->finalLabel->height(), Qt::KeepAspectRatio));
}

void MainWindow::on_actionCorrect_gamma_triggered()
{
    QImage image = current.toImage().convertToFormat(QImage::Format_RGB888);
    uchar* pix = image.bits();
    uchar* end_pix = pix + image.sizeInBytes();

    for (; pix < end_pix; pix++){
        *pix = qBound(0.0, qPow(*pix/255.0, 1.0/1.1) * 255.0, 255.0);
    }

    current = QPixmap::fromImage(image);
    ui->finalLabel->setPixmap(current.scaled(ui->finalLabel->width(), ui->finalLabel->height(), Qt::KeepAspectRatio));
}


void MainWindow::convolute(std::vector<std::vector <int> > kernel){

    int dim_x = kernel.size();
    int dim_y = kernel[0].size();

    int dim_x_half = kernel.size()/2;
    int dim_y_half = kernel[0].size()/2;

    QImage image = current.toImage();
    QImage new_image = current.toImage();

    uchar* pix = image.bits();
    uchar* new_pix = new_image.bits();

    int cols = image.width();

    int weight = 0;
    for (int i = 0; i<dim_x; i++) {
        for (int j = 0; j<dim_y; j++) {
            weight += kernel[i][j];
        }
    }

    if (weight <= 0)
        weight = 1;

    for (int p = 0; p < image.sizeInBytes(); p += 4) {
        for (int ch = 0; ch < 3; ch++) {

            int sum = 0;

            /*
            for (int k = -dim_x_half; k <= dim_x_half; k++) {
                for (int l = -dim_y_half; l <= dim_y_half; l++) {

                    int new_value = p + ch + 4*cols*l + 4*k;

                    if (new_value > 0 && new_value < image.sizeInBytes())
                        sum += pix[new_value] * kernel[k + dim_x_half][l + dim_y_half];
                    else
                        sum += new_pix[p + ch];
                }
            }*/


            for (int k = -dim_x_half; k <= dim_x_half; k++) {
                for (int l = -dim_y_half; l <= dim_y_half; l++) {

                    int new_value = p + ch + 4*cols*l + 4*k;

                    if (new_value > 0 && new_value < image.sizeInBytes())
                        sum += pix[new_value] * kernel[k + dim_x_half][l + dim_y_half];
                    else
                        sum += new_pix[p + ch];
                }
            }

            *(new_pix+p+ch) = qBound(0, (int)(sum/weight), 255);
        }
    }

    current = QPixmap::fromImage(new_image);
    ui->finalLabel->setPixmap(current.scaled(ui->finalLabel->width(), ui->finalLabel->height(), Qt::KeepAspectRatio));

}


void MainWindow::on_actionBox_blur_triggered()
{
    std::vector<std::vector<int>> kernel{{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
    convolute(kernel);
}

void MainWindow::on_actionGaussian_blur_triggered()
{
    std::vector<std::vector<int>> kernel{{0, 1, 0}, {1, 4, 1}, {0, 1, 0}};
    convolute(kernel);
}

void MainWindow::on_actionSharpen_triggered()
{
    std::vector<std::vector<int>> kernel{{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
    convolute(kernel);
}

void MainWindow::on_actionEdge_detection_triggered()
{
    std::vector<std::vector<int>> kernel{{0, -1, 0}, {0, 1, 0}, {0, 0, 0}};
    convolute(kernel);
}

void MainWindow::on_actionEmboss_triggered()
{
    std::vector<std::vector<int>> kernel{{-1, 0, 1}, {-1, 1, 1}, {-1, 0, 1}};
    convolute(kernel);

}

void MainWindow::on_actionCustom_kernel_triggered()
{
    bool okx, oky;
    int x;
    int y;
    x = QInputDialog::getInt(this, tr("Custom kernel"), tr("Kernel dimension of x:"), 3, 1, 9, 2, &okx);

    if (okx){
        y = QInputDialog::getInt(this, tr("Custom kernel"), tr("Kernel dimension of y:"), 3, 1, 9, 2, &oky);

        if (oky) {
            auto custom_kernel = new customkernel(nullptr, x, y);
            custom_kernel->setModal(true);
            int diagCode = custom_kernel->exec();

            if (diagCode == QDialog::Accepted)
                convolute(custom_kernel->values);
            /*
            custom_kernel->get_values();
            std::vector<std::vector<int> > kernel = custom_kernel->values;
            convolute(kernel);
            */

            }
    }
}
