#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDataStream>
#include <QMessageBox>
#include <QtGlobal>
#include <QtConcurrent>
#include <algorithm>

///Подключаем все что нужно для графиков
#include <QLineSeries>
#include <QtCharts>
#include <QChartView>
#include "graphicchart.h"

#define FD 1000.0 //частота дискретизации

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //Метод считывает данные из исходного файла
    QVector<uint32_t> ReadFile(const QString& path, const uint8_t numberChannel);
    //Метод преобразует данные физические величины, В?
    QVector<double> ProcessFile(const QVector<uint32_t>& dataFile);
    //Метод ищет Максимумы
    QVector<double> FindMax(const QVector<double>& resultData);
    //Метод ищет минимумы
    QVector<double> FindMin(const QVector<double>& resultData);


private slots:
    //Метод отображает результаты
    void DisplayResult(const QVector<double>& mins, const QVector<double>& maxs);

    void on_pb_path_clicked();
    void on_pb_start_clicked();
    void ViewGraph();

private:
    Ui::MainWindow *ui;
    QString pathToFile = "";
    uint8_t numberSelectChannel = 0xEA;

    // построение графика
    GraphicChart* graphClass;   // функционал графика
    QChart* chart;              // указатель на график
    QChartView* chartView;      // виджет-отображальщик графика, принимает QChart*
    //QGridLayout *layout;

signals:
    void sig_resultReady(const QVector<double>& mins, const QVector<double>& maxs);
};
#endif // MAINWINDOW_H
