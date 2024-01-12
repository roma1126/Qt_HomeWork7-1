#ifndef GRAPHICCHART_H
#define GRAPHICCHART_H

#include <QObject>

//Подключаем все что нужно для графиков
#include <QLineSeries>
#include <QtCharts>
#include <QChartView>

class GraphicChart : public QObject
{
    Q_OBJECT

private:
    QLineSeries* ptrGraph;

public:
    explicit GraphicChart(QObject *parent = nullptr);

    void AddDataToGrahp(QVector<double> x, QVector<double> y);
    void ClearGraph(QChart* chart);
    void UpdateGraph(QChart* chart);

signals:
    void sig_graphReady();
};

#endif // GRAPHICCHART_H
