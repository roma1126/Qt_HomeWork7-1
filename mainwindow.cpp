#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pb_clearResult->setCheckable(true);
    ui->le_path->setText(pathToFile);

    // построение графика
    // Объект QChart является основным, в котором хранятся все данные графиков и который отвечает
    // за само поле отображения графика, управляет осями, легенодой и прочими атрибутами графика.
    chart = new QChart();
    chart->legend()->setVisible(false);
    // Объект QChartView является виджетом отображальщиком графика. В его конструктор
    // необходимо передать ссылку на объект QChart.
    chartView = new QChartView(chart);
    // Создадим объект нашего класса с функционалом графика.
    graphClass = new GraphicChart(this);
    //chart -> chartVuiew -> данные для отображения

    //layout = new QGridLayout;
    //gr->wid_graph->setLayout(layout);
    //layout->addWidget(chartView);
    //chartView->show();

    connect(this, &MainWindow::sig_resultReady, this, &MainWindow::DisplayResult);
    connect(graphClass, &GraphicChart::sig_graphReady, this, &MainWindow::ViewGraph);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete chart;
    delete chartView;
}



/****************************************************/
/*!
@brief:	Метод считывает данные из файла
@param: path - путь к файлу
        numberChannel - какой канал АЦП считать
*/
/****************************************************/
QVector<uint32_t> MainWindow::ReadFile(const QString& path, const uint8_t numberChannel)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly);

    if(file.isOpen() == false)
    {
        if(pathToFile.isEmpty())
        {
            QMessageBox mb;
            mb.setWindowTitle("Ошибка");
            mb.setText("Ошибка открытия фала");
            mb.exec();
        }
    }
    else
    {
        //продумать как выйти из функции
    }

    QDataStream dataStream;
    dataStream.setDevice(&file);
    dataStream.setByteOrder(QDataStream::LittleEndian);

    QVector<uint32_t> readData;
    readData.clear();
    uint32_t currentWorld = 0, sizeFrame = 0;

    while(dataStream.atEnd() == false)
    {
        dataStream >> currentWorld;

        if(currentWorld == 0xFFFFFFFF)
        {
            dataStream >> currentWorld;

            if(currentWorld < 0x80000000)
            {
                dataStream >> sizeFrame;

                if(sizeFrame > 1500) continue;

                uint32_t worldInSf( sizeFrame/sizeof(uint32_t) );
                for(int i(0); i<worldInSf; ++i)
                {
                    dataStream >> currentWorld;

                    if((currentWorld >> 24) == numberChannel)
                    {
                        readData.append(currentWorld);
                    }
                }
            }
        }
    }
    ui->chB_readSucces->setChecked(true);
    return readData;
}

QVector<double> MainWindow::ProcessFile(const QVector<uint32_t>& dataFile)
{
    QVector<double> resultData;
    resultData.clear();

    foreach (int word, dataFile)
    {
        word &= 0x00FFFFFF;
        if(word > 0x800000) word -= 0x1000000;

        double res = ((double)word / 6000000) * 10;
        resultData.append(res);
    }
    ui->chB_procFileSucces->setChecked(true);

    return resultData;
}

QVector<double> MainWindow::FindMax(const QVector<double>& resultData)
{
    double max = 0, sMax=0;
    //Поиск первого максиума
    foreach (double num, resultData)
    {
        //QThread::usleep(1);
        if(num > max)
        {
            max = num;
        }
    }

    //Поиск 2го максимума
    foreach (double num, resultData)
    {
        //QThread::usleep(1);
        if(num > sMax && (qFuzzyCompare(num, max) == false))
        {
            sMax = num;
        }
    }

    ui->chB_maxSucess->setChecked(true);
    return {max, sMax};
}

QVector<double> MainWindow::FindMin(const QVector<double>& resultData)
{

    double min = 0, sMin = 0;
    //QThread::sleep(1);
    //Поиск первого максиума
    foreach (double num, resultData)
    {
        if(num < min)
        {
            min = num;
        }
    }
    //QThread::sleep(1);
    //Поиск 2го максимума
    foreach (double num, resultData)
    {
        if(num < sMin && (qFuzzyCompare(num, min) == false))
        {
            sMin = num;
        }
    }

    ui->chB_minSucess->setChecked(true);
    return {min, sMin};
}

void MainWindow::DisplayResult(const QVector<double>& mins, const QVector<double>& maxs)
{
    ui->te_Result->append("Расчет закончен!");

    ui->te_Result->append("Первый минимум " + QString::number(mins.first()));
    ui->te_Result->append("Второй минимум " + QString::number(mins.at(1)));

    ui->te_Result->append("Первый максимум " + QString::number(maxs.first()));
    ui->te_Result->append("Второй максимум " + QString::number(maxs.at(1)) + "\n");
}


/****************************************************/
/*!
@brief:	Обработчик клика на кнопку "Выбрать путь"
*/
/****************************************************/
void MainWindow::on_pb_path_clicked()
{
    //pathToFile = "";
    //ui->le_path->clear();

    QString pf = QFileDialog::getOpenFileName(this,
                                              tr("Открыть файл"), "/home/", tr("ADC Files (*.adc)"));
    if (pf != "")
    {
        pathToFile = pf;
        ui->le_path->setText(pathToFile);
    }
}

/****************************************************/
/*!
@brief:	Обработчик клика на кнопку "Старт"
*/
/****************************************************/
void MainWindow::on_pb_start_clicked()
{
    //проверка на то, что файл выбран
    if(pathToFile.isEmpty())
    {
        QMessageBox mb;
        mb.setWindowTitle("Ошибка");
        mb.setText("Выберите файл!");
        mb.exec();
        return;
    }
    graphClass->ClearGraph(chart);

    ui->chB_maxSucess->setChecked(false);
    ui->chB_procFileSucces->setChecked(false);
    ui->chB_readSucces->setChecked(false);
    ui->chB_minSucess->setChecked(false);

    //Маски каналов
    switch (ui->cmB_numCh->currentIndex())
    {
    case 0:
        numberSelectChannel = 0xEA;
        break;

    case 1:
        numberSelectChannel = 0xEF;
        break;

    case 2:
        numberSelectChannel = 0xED;
        break;

    default:
        numberSelectChannel = 0xEA;
        break;
    }

    auto read = [&]{ return ReadFile(pathToFile, numberSelectChannel); };
    auto process = [&](QVector<uint32_t> dataFile){ return ProcessFile(dataFile);};
    auto findMax = [&](QVector<double> dataProcess){
                                                QVector<double> maxs = FindMax(dataProcess);
                                                QVector<double> mins = FindMin(dataProcess);
                                                // пытается вывести результат, до того как его посчитает!
                                                //DisplayResult(mins, maxs); // НЕ правильно!!!
                                                emit sig_resultReady(mins, maxs); // так хорошо работает

                                                /*
                                                 * Тут необходимо реализовать код наполнения серии
                                                 * и вызов сигнала для отображения графика
                                                 */
                                                QVector<double> x, y;
                                                x.resize((uint32_t)FD);
                                                y.resize((uint32_t)FD);
                                                for (uint32_t i(0); i < (uint32_t)FD; ++i)
                                                {
                                                    x[i] = i / FD;          // время в секундах
                                                    y[i] = dataProcess[i];  // данные с АЦП
                                                }
                                                graphClass->AddDataToGrahp(x, y);
                                                graphClass->UpdateGraph(chart);
                                                emit graphClass->sig_graphReady();
                                             };

    auto result = QtConcurrent::run(read)
                               .then(process)
                               .then(findMax);
}

void MainWindow::ViewGraph()
{
    chartView->chart()->createDefaultAxes();
    chartView->resize(600, 300);
    chartView->show();
}
