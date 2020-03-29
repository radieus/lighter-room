#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QTextStream>
#include <QtGlobal>
#include <QtMath>
#include <QInputDialog>

#include "customkernel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString filename;
    QImage finalImage;
    QImage originalImage;
    QPixmap current;

    //customkernel * custom_kernel;

private slots:

    void on_actionSave_as_triggered();

    void on_actionOpen_triggered();

    void on_actionInvert_triggered();

    void on_actionBrighten_triggered();

    void on_actionDarken_triggered();

    void on_actionEnchance_contrast_triggered();

    void on_actionCorrect_gamma_triggered();

    void on_actionRestore_triggered();

    void on_actionBox_blur_triggered();

    void convolute(std::vector<std::vector <int> > kernel, int offset = 0, int divisor = -1);

    void on_actionGaussian_blur_triggered();

    void on_actionSharpen_triggered();

    void on_actionEdge_detection_triggered();

    void on_actionEmboss_triggered();

    void on_actionCustom_kernel_triggered();

    int getClosest(int val1, int val2, int target);

    int findClosest(std::vector<int> arr, int n, int target);

    void error_diffusion(std::vector<std::vector<float> > kernel, int k);

    void on_actionFloyd_and_Steinberg_filter_triggered();

    void on_actionAtkinson_Filter_triggered();

    void on_actionBurkes_Filter_triggered();

    void on_actionStucky_Filter_triggered();

    void on_actionSierra_Filter_triggered();

    void on_actionChange_to_grayscale_triggered();

    void on_actionUniform_Quantization_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
