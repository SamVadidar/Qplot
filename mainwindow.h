#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <qcustomplot.h>

using namespace cv;

#include <iostream>
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //These are the parameters which are used by the functions
    //Is it necessary to add them as public members of a class in which they are called?
    Mat frame;
    cv::Ptr<cv::face::Facemark> facemark;
    bool success;
    std::vector<cv::Rect> faces;
    //cv::CascadeClassifier faceDetector;
    std::vector< std::vector<cv::Point2f> > landmarks;
    float value=0.0;

    void drawLandmarks(cv::Mat &im, std::vector<cv::Point2f> &landmarks);
    void setupRealtimeDataDemo(QCustomPlot *customPlot);


private slots:
    void on_pushButton_open_webcam_clicked();

    void on_pushButton_close_webcam_clicked();

    void update_window();
    void realtimeDataSlot();

private:
    Ui::MainWindow *ui;

    QTimer *timer;
    QTimer dataTimer;
    VideoCapture cap;


    QImage qt_image;
    cv::CascadeClassifier *faceDetector;


};

#endif // MAINWINDOW_H
