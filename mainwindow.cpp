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


void MainWindow::convolute(std::vector<std::vector<int> > kernel, int offset, int divisor) {

    int dim_x = kernel.size();
    int dim_y = kernel[0].size();

    int dim_x_half = kernel.size()/2;
    int dim_y_half = kernel[0].size()/2;

    QImage image = current.toImage();
    QImage new_image = current.toImage();

    uchar* pix = image.bits();
    uchar* new_pix = new_image.bits();

    int cols = image.width();

    if (divisor == -1) {
        for (int i = 0; i<dim_x; i++) {
            for (int j = 0; j<dim_y; j++) {
                divisor += kernel[i][j];
            }
        }

        if (divisor <= 0)
            divisor = 1;
    }

    for (int p = 0; p < image.sizeInBytes(); p += 4) {
        for (int ch = 0; ch < 3; ch++) {

            int sum = 0;

            for (int k = -dim_x_half; k <= dim_x_half; k++) {
                for (int l = -dim_y_half; l <= dim_y_half; l++) {

                    int new_value = p + ch + 4*cols*l + 4*k;

                    if (new_value > 0 && new_value < image.sizeInBytes())
                        sum += pix[new_value] * kernel[k + dim_x_half][l + dim_y_half];
                    else
                        sum += new_pix[p + ch];
                }
            }

            *(new_pix+p+ch) = qBound(0, (sum / divisor) + offset, 255);
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
    convolute(kernel, 127);
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

            if (diagCode == QDialog::Accepted) {
                int offset = custom_kernel->getOffset();
                int divisor = custom_kernel->getDivisor();
                convolute(custom_kernel->values, offset, divisor);
            }
        }
    }
}


int MainWindow::getClosest(int val1, int val2, int target)
{
    if (target - val1 >= val2 - target)
        return val2;
    else
        return val1;
}

int MainWindow::findClosest(std::vector<int> vec, int n, int target)
{
    if (target <= vec[0])
        return vec[0];
    if (target >= vec[n - 1])
        return vec[n - 1];

    int i = 0, j = n, mid = 0;
    while (i < j) {
        mid = (i + j) / 2;

        if (vec[mid] == target)
            return vec[mid];

        if (target < vec[mid]) {
            if (mid > 0 && target > vec[mid - 1])
                return getClosest(vec[mid - 1], vec[mid], target);
            j = mid;
        }
        else {
            if (mid < n - 1 && target < vec[mid + 1])
                return getClosest(vec[mid], vec[mid + 1], target);
            i = mid + 1;
        }
    }

    return vec[mid];
}


void MainWindow::error_diffusion(std::vector<std::vector<float> > matrix, int k) {

    QImage image = current.toImage();
    QImage new_image = current.toImage();

    uchar* pix = image.bits();
    uchar* new_pix = new_image.bits();

    int f_x_half = matrix.size() / 2;
    int f_y_half = matrix[0].size() / 2;

    std::vector<int> levels;
    double breaks = 1.0/k;
    for (int i = 0; i < k + 1; ++i) {
        levels.push_back(std::floor(i * breaks * 255));
    }

    int cols = image.width();
    int error = 0;

    for (int ch = 0; ch < 3; ch++) {
        for (int p = ch; p < image.sizeInBytes(); p += 4) {

            int approx = findClosest(levels, levels.size(), *(pix + p));
            error = *(pix + p) - approx;
            *(new_pix + p) = approx;

            for (int i = -f_x_half; i <= f_x_half; ++i ) {
                for (int j = -f_y_half; j <= f_y_half; ++j ) {

                   if (matrix[i + f_x_half][j + f_y_half] == 0)
                        continue;

                   if (p + 4*cols*j + 4*i < image.sizeInBytes() && p + 4*cols*j + 4*i >= 0)
                        *(pix + p + 4*cols*j + 4*i) = qBound(0.0, (double)*(pix + p + 4*cols*j + 4*i) + error * matrix[i + f_x_half][j + f_y_half], 255.0);

                }
            }
        }
    }

    current = QPixmap::fromImage(new_image);
    ui->finalLabel->setPixmap(current.scaled(ui->finalLabel->width(), ui->finalLabel->height(), Qt::KeepAspectRatio));

}

void MainWindow::on_actionFloyd_and_Steinberg_filter_triggered() //3x3
{
    bool ok;
    int k;

    k = QInputDialog::getInt(this, tr("Levels"), tr("Number of levels:"), 2, 1, 20, 1, &ok);

    if (ok){
        std::vector<std::vector<float> > matrix{{0.0, 0.0, 0.0},
                                                {0.0, 0.0, 7.0/16.0},
                                                {3.0/16.0, 5.0/16.0, 1.0/16.0}};
        error_diffusion(matrix, k - 1);
    }
}

void MainWindow::on_actionBurkes_Filter_triggered() //3x5
{
    bool ok;
    int k;

    k = QInputDialog::getInt(this, tr("Levels"), tr("Number of levels:"), 2, 1, 20, 1, &ok);

    if (ok){
    std::vector<std::vector<float>> matrix{{0.0, 0.0, 0.0, 0.0, 0.0},
                                           {0.0, 0.0, 0.0, 8.0/32.0, 4.0/32.0},
                                           {2.0/32.0, 4.0/32.0, 8.0/32.0, 4.0/32.0, 2.0/32.0}};
    error_diffusion(matrix, k - 1);
    }
}

void MainWindow::on_actionStucky_Filter_triggered() //5x5
{
    bool ok;
    int k;

    k = QInputDialog::getInt(this, tr("Levels"), tr("Number of levels:"), 2, 1, 20, 1, &ok);

    if (ok){
    std::vector<std::vector<float>> matrix{{0.0, 0.0, 0.0, 0.0, 0.0},
                                           {0.0, 0.0, 0.0, 0.0, 0.0},
                                           {0.0, 0.0, 0.0, 8.0/42.0, 4.0/42.0},
                                           {2.0/42.0, 4.0/42.0, 8.0/42.0, 4.0/42.0, 2.0/42.0},
                                           {1.0/42.0, 2.0/42.0, 4.0/42.0, 2.0/42.0, 1.0/42.0}};
    error_diffusion(matrix, k - 1);
    }
}

void MainWindow::on_actionSierra_Filter_triggered() //5x5
{
    bool ok;
    int k;

    k = QInputDialog::getInt(this, tr("Levels"), tr("Number of levels:"), 2, 1, 20, 1, &ok);

    if (ok){
    std::vector<std::vector<float>> matrix{{0.0, 0.0, 0.0, 0.0, 0.0},
                                           {0.0, 0.0, 0.0, 0.0, 0.0},
                                           {0.0, 0.0, 0.0, 5.0/32.0, 3.0/32.0},
                                           {2.0/32.0, 4.0/32.0, 5.0/32.0, 4.0/32.0, 2.0/32.0},
                                           {0.0, 2.0/32.0, 3.0/32.0, 2.0/32.0, 0.0}};
    error_diffusion(matrix, k - 1);
    }
}

void MainWindow::on_actionAtkinson_Filter_triggered() //5x5
{
    bool ok;
    int k;

    k = QInputDialog::getInt(this, tr("Levels"), tr("Number of levels:"), 2, 1, 20, 1, &ok);

    if (ok){
    std::vector<std::vector<float>> matrix{{0.0, 0.0, 0.0, 0.0, 0.0},
                                           {0.0, 0.0, 0.0, 0.0, 0.0},
                                           {0.0, 0.0, 0.0, 1.0/8.0, 1.0/8.0},
                                           {0.0, 1.0/8.0, 1.0/8.0, 1.0/8.0, 0.0},
                                           {0.0, 0.0, 1.0/8.0, 0.0, 0.0}};
    error_diffusion(matrix, k - 1);
    }
}

void MainWindow::on_actionChange_to_grayscale_triggered()
{
    QImage image = current.toImage();
    uchar* pix = image.bits();
    uchar* end = pix + 4 * image.width() * image.height();

    for(; pix < end; pix += 4){
        int color = *pix * 0.3 + *(pix+1) * 0.6 + *(pix+2) * 0.1;
        *pix = color;
        *(pix+1) = color;
        *(pix+2) = color;
      }

    current = QPixmap::fromImage(image);
    ui->finalLabel->setPixmap(current.scaled(ui->finalLabel->width(), ui->finalLabel->height(), Qt::KeepAspectRatio));

}

void MainWindow::on_actionUniform_Quantization_triggered()
{
    bool ok;
    QImage image = current.toImage();
    uchar* pix = image.bits();

    int divisions;
    divisions = QInputDialog::getInt(this, tr("Intervals"), tr("Number of intervals:"), 2, 1, 20, 1, &ok);

    if (ok){

        double breaks = 256.0/divisions;
        for (int p = 0; p < image.sizeInBytes(); p += 4) {
            for (int ch = 0; ch < 3; ch++) {

                *(pix+p+ch)= ((int)(*(pix+p+ch)/breaks) + 0.5)*breaks;

            }
        }
    }

    current = QPixmap::fromImage(image);
    ui->finalLabel->setPixmap(current.scaled(ui->finalLabel->width(), ui->finalLabel->height(), Qt::KeepAspectRatio));

}
