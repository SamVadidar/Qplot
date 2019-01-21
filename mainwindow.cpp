#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#define COLOR cv::Scalar(0, 0, 255)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setGeometry(400, 250, 542, 390);
    //void MainWindow::setupRealtimeDataDemo(Qcustomplot *customplot);
    setupRealtimeDataDemo(ui->customplot);
    setWindowTitle("Qcustomplot: Real time");
    statusBar()->clearMessage();
    ui->customplot->replot();

    timer = new QTimer(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_close_webcam_clicked()
{
    disconnect(timer, SIGNAL(timeout()), this, SLOT(update_window()));
    cap.release();

    Mat image = Mat::zeros(frame.size(),CV_8UC3);

    qt_image = QImage((const unsigned char*) (image.data), image.cols, image.rows, QImage::Format_RGB888);

    ui->label->setPixmap(QPixmap::fromImage(qt_image));

    ui->label->resize(ui->label->pixmap()->size());

    cout << "camera is closed" << endl;
}

void MainWindow::on_pushButton_open_webcam_clicked()
{
    cap.open(0);
    facemark = cv::face::FacemarkLBF::create();
    facemark->loadModel("/home/sam/Downloads/asd/OpenCV-CPP-show-webcam-stream-on-Qt-GUI-master/OpenCV-CPP-show-webcam-stream-on-Qt-GUI-master/lbfmodel.yaml");



    if(!cap.isOpened())  // Check if we succeeded
    {
        cout << "camera is not open" << endl;
    }
    else
    {
        cout << "camera is open" << endl;
        faceDetector = new cv::CascadeClassifier("/home/sam/Downloads/asd/OpenCV-CPP-show-webcam-stream-on-Qt-GUI-master/OpenCV-CPP-show-webcam-stream-on-Qt-GUI-master/haarcascade_frontalface_alt2.xml");

        connect(timer, SIGNAL(timeout()), this, SLOT(update_window()));

        timer->start(20);
    }
}

void MainWindow::update_window()

{
    cap >> frame;
    faceDetector->detectMultiScale(frame, faces);
    success = facemark->fit(frame, faces, landmarks);

    if (success)
    {

        value = landmarks[0][30].x;
        for(int i = 0; i <landmarks.size(); i++)
        {
            drawLandmarks(frame, landmarks[i]);
        }
    }
    cvtColor(frame, frame, CV_BGR2RGB);

    qt_image = QImage((const unsigned char*) (frame.data), frame.cols, frame.rows, QImage::Format_RGB888);

    ui->label->setPixmap(QPixmap::fromImage(qt_image));

    ui->label->resize(ui->label->pixmap()->size());
}

void MainWindow::drawLandmarks(cv::Mat &im, std::vector<cv::Point2f> &landmarks)
{
    for(int i = 0; i < landmarks.size(); i++)
    {
      cv::circle(im,landmarks[i],1, COLOR, cv::FILLED);
     // std::cout << "(Point" <<i+1<<":)" <<landmarks[i] << std::endl;
    }
}

void MainWindow::setupRealtimeDataDemo(QCustomPlot *customplot)
{

  // include this section to fully disable antialiasing for higher performance:
  /*
  customplot->setNotAntialiasedElements(QCP::aeAll);
  QFont font;
  font.setStyleStrategy(QFont::NoAntialias);
  customplot->xAxis->setTickLabelFont(font);
  customplot->yAxis->setTickLabelFont(font);
  customplot->legend->setFont(font);
  */
  customplot->addGraph(); // blue line
  customplot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
  customplot->addGraph(); // red line
  customplot->graph(1)->setPen(QPen(QColor(255, 110, 40)));

  QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
  timeTicker->setTimeFormat("%h:%m:%s");
  customplot->xAxis->setTicker(timeTicker);
  customplot->axisRect()->setupFullAxesBox();
  customplot->yAxis->setRange(0, 640);

  // make left and bottom axes transfer their ranges to right and top axes:
  connect(customplot->xAxis, SIGNAL(rangeChanged(QCPRange)), customplot->xAxis2, SLOT(setRange(QCPRange)));
  connect(customplot->yAxis, SIGNAL(rangeChanged(QCPRange)), customplot->yAxis2, SLOT(setRange(QCPRange)));

  // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
  connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
  dataTimer.start(0); // Interval 0 means to refresh as fast as possible
}

void MainWindow::realtimeDataSlot()
{
  static QTime time(QTime::currentTime());
  // calculate two new data points:
  double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
  static double lastPointKey = 0;
  if (key-lastPointKey > 0.002) // at most add point every 2 ms
  {
    // add data to lines:
//    ui->customplot->graph(0)->addData(key, qSin(key)+qrand()/(double)RAND_MAX*1*qSin(key/0.3843));
//    ui->customplot->graph(1)->addData(key, qCos(key)+qrand()/(double)RAND_MAX*0.5*qSin(key/0.4364));
      if(success) ui->customplot->graph(0)->addData(key, value);
    // rescale value (vertical) axis to fit the current data:
    //ui->customplot->graph(0)->rescaleValueAxis();
    //ui->customplot->graph(1)->rescaleValueAxis(true);
    lastPointKey = key;
  }
  // make key axis range scroll with the data (at a constant range size of 8):
  ui->customplot->xAxis->setRange(key, 8, Qt::AlignRight);
  ui->customplot->replot();

  // calculate frames per second:
  static double lastFpsKey;
  static int frameCount;
  ++frameCount;
  if (key-lastFpsKey > 2) // average fps over 2 seconds
  {
    ui->statusBar->showMessage(
          QString("%1 FPS, Total Data points: %2")
          .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
          .arg(ui->customplot->graph(0)->data()->size()+ui->customplot->graph(1)->data()->size())
          , 0);
    lastFpsKey = key;
    frameCount = 0;
  }
}
