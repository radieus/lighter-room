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

    void convolute(std::vector<std::vector <int> > kernel);

    void on_actionGaussian_blur_triggered();

    void on_actionSharpen_triggered();

    void on_actionEdge_detection_triggered();

    void on_actionEmboss_triggered();

    void on_actionCustom_kernel_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
