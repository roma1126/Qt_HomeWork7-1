#include "graphicchart.h"

GraphicChart::GraphicChart(QObject *parent)
    : QObject{parent}
{
    ptrGraph = new QLineSeries(this);   // объект серий
}

/****************************************************/
// Добавляем данные на график
/****************************************************/
void GraphicChart::AddDataToGrahp(QVector<double> x, QVector<double> y)
{
    //Зададим размер графика, т.е. количество точек.
    //Необходимо, чтобы х и у были равны.
    // РАВНЫ, т.к. каждой точке соотв. 1ин "x" и 1ин "y", т.е. пара координат
    uint32_t size = (x.size() >= y.size()) ? y.size() : x.size();

    //Добавление точек в серию.
    for(uint32_t i(0); i < size; ++i)
    {
        ptrGraph->append(x[i],y[i]);
    }
}

/****************************************************/
// Очищаем данные на графике
// *chart - указатель на график
/****************************************************/
void GraphicChart::ClearGraph(QChart *chart)
{
    ptrGraph->clear();              // очистка
    if ( !(chart->series().empty()) )
    {
        chart->removeSeries(ptrGraph);  // удаление серии
    }
}

/****************************************************/
// Добавляем серию к графику
// *chart - указатель на график
/****************************************************/
void GraphicChart::UpdateGraph(QChart *chart)
{
    chart->addSeries(ptrGraph);
    //emit sig_graphReady();
}
