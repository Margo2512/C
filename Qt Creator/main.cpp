#include <QApplication>
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QSurfaceDataItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QMainWindow>
#include <QPainter>
#include <QProgressBar>
#include <QLabel>
#include <cmath>
#include <QStatusBar>
#include <QPushButton>
#include <QCheckBox>
#include <QApplication>
#include <QWidget>
#include <Q3DSurface>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QScreen>

int sampleCountX = 50;
int sampleCountZ = 50;
float xMin = -10.0f;
float xMax = 10.0f;
float zMin = -10.0f;
float zMax = 10.0f;
float xStep = (xMax - xMin) / (sampleCountX - 1);
float zStep = (zMax - zMin) / (sampleCountZ - 1);

bool noCoordGraphGenerated = false;
bool withCoordGraphGenerated = false;
bool flag = false;

float sinc(float R) {
    if (R == 0.0f)
        return 1.0f;
    else
        return (qSin(R) / R + 0.24f) * 1.61f;
}

void handlePointSelected(const QPoint& point, QStatusBar* statusBar, QSurface3DSeries* series) {
    if (noCoordGraphGenerated || !withCoordGraphGenerated) {
        statusBar->clearMessage();
        return;
    }
    //Индексы
    int xIndex = point.x();
    int zIndex = point.y();

    // реальные значения координат
    float xValue = xMin + xIndex * xStep;
    float zValue = zMin + zIndex * zStep;

    QSurfaceDataProxy *dataProxy = series->dataProxy();
    const QSurfaceDataItem *item = dataProxy->itemAt(xIndex, zIndex);
    if (item) {
        float yValue = item->position().y();

        // координаты в statusBar с двумя знаками после запятой
        QString coordinates = QString("Coordinates: X=%1, Y=%2, Z=%3")
                                  .arg(QString::number(xValue, 'f', 2))
                                  .arg(QString::number(yValue, 'f', 2))
                                  .arg(QString::number(zValue, 'f', 2));
        statusBar->showMessage(coordinates);
    }
}


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QWidget *widget = new QWidget;

    Q3DSurface *graph = new Q3DSurface();

    graph->activeTheme()->setGridEnabled(false);

//     Окно для отображения графика
    QWidget *window = QWidget::createWindowContainer(graph);
    window->resize(800, 600);

    //ПЕРВЫЙ БЛОК КНОПОК
    QRadioButton *sincDistanceButton = new QRadioButton("sinc(distance_from_zero)");
    QRadioButton *sincProductButton = new QRadioButton("sinc(x) * sinc(z)");

    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->addWidget(sincDistanceButton);
    buttonLayout->addWidget(sincProductButton);
//    window->addWidget(buttonLayout);

    QWidget *groupBoxWidget = new QWidget();
    groupBoxWidget->setLayout(buttonLayout);

    QGroupBox *groupBox = new QGroupBox("Plot");
    groupBox->setStyleSheet("QGroupBox { border: 2px solid gray; border-radius: 5px; }");
    groupBox->setLayout(new QVBoxLayout());
    groupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    groupBox->setMinimumWidth(200);
    groupBox->setMaximumWidth(200);
    groupBox->setFlat(true);
    groupBox->setContentsMargins(0, 0, 0, 0);
    groupBox->setLayout(new QVBoxLayout());
    groupBox->layout()->addWidget(groupBoxWidget);

    //ВТОРОЙ БЛОК КНОПОК
    QRadioButton *noCoord = new QRadioButton("No selection");
    QRadioButton *withCoord = new QRadioButton("Item");

    QVBoxLayout *buttonLayout2 = new QVBoxLayout();
    buttonLayout2->addWidget(noCoord);
    buttonLayout2->addWidget(withCoord);

    QWidget *groupBoxWidget2 = new QWidget();
    groupBoxWidget2->setLayout(buttonLayout2);

    QGroupBox *groupBox2 = new QGroupBox("Selection Mode");
    groupBox2->setStyleSheet("QGroupBox { border: 2px solid gray; border-radius: 5px; }");
    groupBox2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    groupBox2->setMinimumWidth(200);
    groupBox2->setMaximumWidth(200);
    groupBox2->setFlat(true);
    groupBox2->setContentsMargins(0, 0, 0, 0);
    groupBox2->setLayout(new QVBoxLayout());
    groupBox2->layout()->addWidget(groupBoxWidget2);

    //ТРЕТИЙ БЛОК КНОПОК
    //ГРАДИЕНТ
    QLinearGradient grBtoY(0, 0, 1, 100);
    grBtoY.setColorAt(0.77, Qt::yellow);
    grBtoY.setColorAt(0.47, Qt::red);
    grBtoY.setColorAt(0.25, Qt::blue);
    QPixmap pmBtoY(24, 100);
    QPainter pmpBtoY(&pmBtoY);
    pmpBtoY.setBrush(QBrush(grBtoY));
    pmpBtoY.setPen(Qt::NoPen);
    pmpBtoY.drawRect(0, 0, 24, 100);
    QPushButton *gradientBtoYPB = new QPushButton(widget);
    gradientBtoYPB->setIcon(QIcon(pmBtoY));
    gradientBtoYPB->setIconSize(QSize(24, 100));

    QLinearGradient grBtoRGB(0, 0, 1, 100);
    grBtoRGB.setColorAt(0.77, Qt::blue);
    grBtoRGB.setColorAt(0.47, Qt::green);
    grBtoRGB.setColorAt(0.25, Qt::red);
    QPixmap pmBtoRGB(24, 100);
    QPainter pmpBtoRGB(&pmBtoRGB);
    pmpBtoRGB.setBrush(QBrush(grBtoRGB));
    pmpBtoRGB.setPen(Qt::NoPen);
    pmpBtoRGB.drawRect(0, 0, 24, 100);
    QPushButton *gradientBtoRGB = new QPushButton(widget);
    gradientBtoRGB->setIcon(QIcon(pmBtoRGB));
    gradientBtoRGB->setIconSize(QSize(24, 100));


    QHBoxLayout *buttonLayout3 = new QHBoxLayout();
    buttonLayout3->addWidget(gradientBtoYPB);
    buttonLayout3->addWidget(gradientBtoRGB);
    buttonLayout3->setAlignment(Qt::AlignTop);

    QWidget *groupBoxWidget3 = new QWidget();
    groupBoxWidget3->setLayout(buttonLayout3);

    QGroupBox *groupBox3 = new QGroupBox("Custom gradient");
    groupBox3->setStyleSheet("QGroupBox { border: 2px solid gray; border-radius: 5px; }");
    groupBox3->setLayout(new QVBoxLayout());
    groupBox3->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    groupBox3->setMinimumWidth(200);
    groupBox3->setMaximumWidth(200);
    groupBox3->setAlignment(Qt::AlignTop);
    groupBox3->setFlat(true);
    groupBox3->setContentsMargins(0, 0, 0, 0);
    groupBox3->setLayout(new QVBoxLayout());
    groupBox3->layout()->addWidget(groupBoxWidget3);

    //ЧЕТВЕРТЫЙ БЛОК
    QSlider* rangeXMinSlider1 = new QSlider(Qt::Horizontal);
        rangeXMinSlider1->setRange(-100, 100);
        rangeXMinSlider1->setValue(-100);
    QSlider* rangeXMaxSlider1 = new QSlider(Qt::Horizontal);
            rangeXMaxSlider1->setRange(-100, 100);
            rangeXMaxSlider1->setValue(100);

    QLabel *rangeXLabel1 = new QLabel(QString::number(xMin));
    QLabel *rangeXLabel2 = new QLabel(QString::number(xMax));

    QHBoxLayout *buttonLayout4 = new QHBoxLayout();
    buttonLayout4->addWidget(rangeXMinSlider1);
    buttonLayout4->addWidget(rangeXMaxSlider1);
    buttonLayout4->addWidget(rangeXLabel1);
    buttonLayout4->addWidget(rangeXLabel2);

    QWidget *groupBoxWidget4 = new QWidget();
    groupBoxWidget4->setLayout(buttonLayout4);

    QGroupBox *groupBox4 = new QGroupBox("Column range");
    groupBox4->setStyleSheet("QGroupBox { border: 2px solid gray; border-radius: 5px; }");
    groupBox4->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    groupBox4->setMinimumWidth(200);
    groupBox4->setMaximumWidth(200);
    groupBox4->setFlat(true);
    groupBox4->setContentsMargins(0, 0, 0, 0);
    groupBox4->setLayout(new QVBoxLayout());
    groupBox4->layout()->addWidget(groupBoxWidget4);

    //ПЯТЫЙ БЛОК
    QSlider* rangeZMinSlider1 = new QSlider(Qt::Horizontal);
    rangeZMinSlider1->setRange(-100, 100);
    rangeZMinSlider1->setValue(-100);
    QSlider* rangeZMaxSlider1 = new QSlider(Qt::Horizontal);
    rangeZMaxSlider1->setRange(-100, 100);
    rangeZMaxSlider1->setValue(100);

    QLabel *rangeZLabel3 = new QLabel(QString::number(zMin));
    QLabel *rangeZLabel4 = new QLabel(QString::number(zMax));

    QHBoxLayout *buttonLayout5 = new QHBoxLayout();
    buttonLayout5->addWidget(rangeZMinSlider1);
    buttonLayout5->addWidget(rangeZMaxSlider1);
    buttonLayout5->addWidget(rangeZLabel3);
    buttonLayout5->addWidget(rangeZLabel4);

    QWidget *groupBoxWidget5 = new QWidget();
    groupBoxWidget5->setLayout(buttonLayout5);

    QGroupBox *groupBox5 = new QGroupBox("Row range");
    groupBox5->setStyleSheet("QGroupBox { border: 2px solid gray; border-radius: 5px; }");
    groupBox5->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    groupBox5->setMinimumWidth(200);
    groupBox5->setMaximumWidth(200);
    groupBox5->setFlat(true);
    groupBox5->setContentsMargins(0, 0, 0, 0);
    groupBox5->setLayout(new QVBoxLayout());
    groupBox5->layout()->addWidget(groupBoxWidget5);

    QCheckBox *gridCheckBox = new QCheckBox("Show grid");
    QCheckBox *labelCheckBox = new QCheckBox("Show label");
    QCheckBox *labelBorderCheckBox = new QCheckBox("Show label border");

    graph->activeTheme()->setLabelBorderEnabled(false);


    // Создаем кнопку "Save"
    //ЧЕТВЕРТЫЙ БЛОК
    QPushButton *saveButton = new QPushButton("Save");

    QVBoxLayout *buttonLayout6 = new QVBoxLayout();
    buttonLayout6->addWidget(saveButton);

    QGroupBox *groupBox6 = new QGroupBox("");
    groupBox6->setStyleSheet("QGroupBox { border: 2px solid gray; border-radius: 5px; }");
    groupBox6->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    groupBox5->setMinimumWidth(200);
    groupBox5->setMaximumWidth(200);
    groupBox6->setFlat(true);
    groupBox6->setContentsMargins(0, 0, 0, 0);
    groupBox6->setLayout(new QVBoxLayout());
    groupBox6->layout()->addWidget(saveButton);

    QVBoxLayout *buttonBlocksLayout = new QVBoxLayout();
    buttonBlocksLayout->addWidget(gridCheckBox);
    buttonBlocksLayout->addWidget(labelCheckBox);
    buttonBlocksLayout->addWidget(labelBorderCheckBox);
    buttonBlocksLayout->addWidget(groupBox);
    buttonBlocksLayout->addWidget(groupBox2);
    buttonBlocksLayout->addWidget(groupBox4);
    buttonBlocksLayout->addWidget(groupBox5);
    buttonBlocksLayout->addWidget(groupBox3);
    buttonBlocksLayout->addWidget(groupBox6);

    QValue3DAxis *axisX = new QValue3DAxis();
    QValue3DAxis *axisY = new QValue3DAxis();
    QValue3DAxis *axisZ = new QValue3DAxis();
    axisX->setTitle("X Axis");
    axisY->setTitle("Y Axis");
    axisZ->setTitle("Z Axis");
    graph->setAxisX(axisX);
    graph->setAxisY(axisY);
    graph->setAxisZ(axisZ);

    graph->axisX()->setLabelFormat("");
    graph->axisY()->setLabelFormat("");
    graph->axisZ()->setLabelFormat("");

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addLayout(buttonBlocksLayout);
    layout->addWidget(window);

    QMainWindow mainWindow;
    QWidget *mainWidget = new QWidget(&mainWindow);
    mainWidget->setLayout(layout);
    mainWidget->resize(800, 600);
    mainWindow.setCentralWidget(mainWidget);
    mainWindow.setWindowTitle("Graph Visualization");
    mainWidget->show();

    QStatusBar *statusBar = new QStatusBar(&mainWindow);
    mainWindow.setStatusBar(statusBar);
    mainWindow.show();

    QSurfaceDataProxy *proxy = new QSurfaceDataProxy();
    QSurface3DSeries *series = new QSurface3DSeries(proxy);
    graph->addSeries(series);

    QObject::connect(rangeXMinSlider1, &QSlider::valueChanged, [graph, rangeXMinSlider1, rangeXLabel1]() {
        float xMin = (rangeXMinSlider1->value() / 100.0f) * 10.0f;

        graph->axisX()->setRange(xMin, graph->axisX()->max());

        rangeXLabel1->setText(QString::number(xMin));
    });

    QObject::connect(rangeXMaxSlider1, &QSlider::valueChanged, [graph, rangeXMaxSlider1, rangeXLabel2]() {
        float xMax = (rangeXMaxSlider1->value() / 100.0f) * 10.0f;

        graph->axisX()->setRange(graph->axisX()->min(), xMax);

        rangeXLabel2->setText(QString::number(xMax));
    });

    QObject::connect(rangeZMinSlider1, &QSlider::valueChanged, [graph, rangeZMinSlider1, rangeZLabel3]() {
        float zMin = (rangeZMinSlider1->value() / 100.0f) * 10.0f;

        graph->axisZ()->setRange(zMin, graph->axisZ()->max());

        rangeZLabel3->setText(QString::number(zMin));
    });

    QObject::connect(rangeZMaxSlider1, &QSlider::valueChanged, [graph, rangeZMaxSlider1, rangeZLabel4]() {
        float zMax = (rangeZMaxSlider1->value() / 100.0f) * 10.0f;

        graph->axisZ()->setRange(graph->axisZ()->min(), zMax);

        rangeZLabel4->setText(QString::number(zMax));
    });

    QObject::connect(gridCheckBox, &QCheckBox::stateChanged, [graph](int state) {
        graph->activeTheme()->setGridEnabled(state == Qt::Checked);
        flag = true;
    });
    QObject::connect(labelCheckBox, &QCheckBox::stateChanged, [graph](int state) {
            graph->axisX()->setTitleVisible(state == Qt::Checked);
            graph->axisY()->setTitleVisible(state == Qt::Checked);
            graph->axisZ()->setTitleVisible(state == Qt::Checked);
            if (state == Qt::Checked) {
                graph->axisX()->setLabelFormat(QStringLiteral("%.2f"));
                graph->axisY()->setLabelFormat(QStringLiteral("%.2f"));
                graph->axisZ()->setLabelFormat(QStringLiteral("%.2f"));
            } else {
                graph->axisX()->setLabelFormat("");
                graph->axisY()->setLabelFormat("");
                graph->axisZ()->setLabelFormat("");
            }
    });
    QObject::connect(labelBorderCheckBox, &QCheckBox::stateChanged, [graph](int state) {
        if (state == Qt::Checked) {
            graph->activeTheme()->setLabelBorderEnabled(true);
        } else {
            graph->activeTheme()->setLabelBorderEnabled(false);
        }
    });

    bool sincDistanceGraphGenerated = false;
    bool sincProductGraphGenerated = false;

    QObject::connect(sincDistanceButton, &QRadioButton::clicked, [&series, &sincDistanceGraphGenerated, &sincProductGraphGenerated]() {
        if (!sincDistanceGraphGenerated) {
            if (sincProductGraphGenerated) {
                series->dataProxy()->removeRows(0, series->dataProxy()->rowCount());
                sincProductGraphGenerated = false;
            }

            const int sampleCountX = 50;
            const int sampleCountZ = 50;
            const float sampleMin = -8.0f;
            const float sampleMax = 8.0f;

            float stepX = (sampleMax - sampleMin) / float(sampleCountX - 1);
            float stepZ = (sampleMax - sampleMin) / float(sampleCountZ - 1);

            QSurfaceDataArray *dataArray = new QSurfaceDataArray;
            dataArray->reserve(sampleCountZ);

            for (int i = 0; i < sampleCountZ; i++) {
                QSurfaceDataRow *newRow = new QSurfaceDataRow(sampleCountX);
                float z = qMin(sampleMax, (i * stepZ + sampleMin));
                int index = 0;
                for (int j = 0; j < sampleCountX; j++) {
                    float x = qMin(sampleMax, (j * stepX + sampleMin));
                    float R = qSqrt(z * z + x * x) + 0.01f;
                    float y = sinc(R);
                    (*newRow)[index++].setPosition(QVector3D(x, y, z));
                }
                *dataArray << newRow;
            }

            series->dataProxy()->resetArray(dataArray);

            sincDistanceGraphGenerated = true;
        }
    });

    QObject::connect(sincProductButton, &QRadioButton::clicked, [&series, &sincDistanceGraphGenerated, &sincProductGraphGenerated]() {
        if (!sincProductGraphGenerated) {
            if (sincDistanceGraphGenerated) {
                series->dataProxy()->removeRows(0, series->dataProxy()->rowCount());
                sincDistanceGraphGenerated = false;
            }

            const int sampleCountX = 50;
            const int sampleCountZ = 50;
            const float sampleMin = -8.0f;
            const float sampleMax = 8.0f;

            float stepX = (sampleMax - sampleMin) / float(sampleCountX - 1);
            float stepZ = (sampleMax - sampleMin) / float(sampleCountZ - 1);

            QSurfaceDataArray *dataArray = new QSurfaceDataArray;
            dataArray->reserve(sampleCountZ);
            float scaleFactor = 0.5f; // Коэффициент масштабирования

            for (int i = 0; i < sampleCountZ; i++) {
                QSurfaceDataRow *newRow = new QSurfaceDataRow(sampleCountX);
                float z = qMin(sampleMax, (i * stepZ + sampleMin));
                int index = 0;
                for (int j = 0; j < sampleCountX; j++) {
                    float x = qMin(sampleMax, (j * stepX + sampleMin));
                    float y = scaleFactor * sinc(z) * sinc(x);
                    (*newRow)[index++].setPosition(QVector3D(x, y, z));
                }
                *dataArray << newRow;
            }

            series->dataProxy()->resetArray(dataArray);

            sincProductGraphGenerated = true;
        }
    });

    QObject::connect(withCoord, &QRadioButton::toggled, [&sincDistanceGraphGenerated, &sincProductGraphGenerated, graph, statusBar, &series]() {
        graph->setSelectionMode(QAbstract3DGraph::SelectionItem);
        if (!withCoordGraphGenerated) {
            if (noCoordGraphGenerated) {
                noCoordGraphGenerated = false;
            }
            withCoordGraphGenerated = true;
        }
    });

    QObject::connect(noCoord, &QRadioButton::toggled, [&series, &sincDistanceGraphGenerated, &sincProductGraphGenerated, statusBar, graph]() {
        graph->setSelectionMode(QAbstract3DGraph::SelectionNone);

        // Очистите содержимое statusBar
        statusBar->clearMessage();
        if (!noCoordGraphGenerated) {
            if (withCoordGraphGenerated) {
                withCoordGraphGenerated = false;
                statusBar->clearMessage();
            }
            noCoordGraphGenerated = true;
        }
    });

    QObject::connect(gradientBtoYPB, &QPushButton::clicked, [=]() {
        QLinearGradient gr;
        gr.setColorAt(0.33, Qt::blue);
        gr.setColorAt(0.67, Qt::red);
        gr.setColorAt(1.0, Qt::yellow);

        series->setBaseGradient(gr);
        series->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    });

    QObject::connect(gradientBtoRGB, &QPushButton::clicked, [=]() {
        QLinearGradient gr;
        gr.setColorAt(0.0, Qt::red);
        gr.setColorAt(0.33, Qt::green);
        gr.setColorAt(0.67, Qt::blue);

        series->setBaseGradient(gr);
        series->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    });

    QObject::connect(graph->seriesList().at(0), &QSurface3DSeries::selectedPointChanged, [=](const QPoint& point) {
        handlePointSelected(point, statusBar, series);
    });

    auto saveGraph = [=]() {
        QPixmap pixmap = mainWidget->grab();

        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString fileName = currentDateTime.toString("yyyy-MM-dd_hh-mm-ss") + ".png";

        QString filter = "Images (*.png)";

        QString filePath = QFileDialog::getSaveFileName(widget, "Save Graph", fileName, filter);

        if (!filePath.isEmpty()) {
            if (pixmap.save(filePath)) {
                QMessageBox::information(widget, "Save Graph", "Graph saved successfully!");
            } else {
                QMessageBox::warning(widget, "Save Graph", "Failed to save the graph.");
            }
        } else {
        }
    };

    QObject::connect(saveButton, &QPushButton::clicked, saveGraph);

    return a.exec();
}
