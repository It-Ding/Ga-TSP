#include "QtWidgetsApplication1.h"
#include "Ga.h"

QtWidgetsApplication1::QtWidgetsApplication1(QWidget *parent)
    : QMainWindow(parent) {
    ui.setupUi(this);
    QMenuBar *bar = menuBar();
    this->setMenuBar(bar);
    QMenu *fileMenu = bar->addMenu("文件");
    filePath = "cityList.in";

    QAction *newAction = fileMenu->addAction("打开文件");
    connect(newAction, &QAction::triggered, this, &QtWidgetsApplication1::openfile);

    List = new QListWidget(this);
    List->move(500, 90);
    List->resize(150, 470);

    initList((int)routes.size());
    connect(List, &QListWidget::itemClicked, this, &QtWidgetsApplication1::updateMap);

    QPushButton *Btn = new QPushButton("随机生成数据", this);
    QPushButton *Btn1 = new QPushButton("重新训练", this);
    Btn->move(30, 50);
    Btn1->move(500, 50);
    this->resize(800, 1500);
    connect(Btn, &QPushButton::clicked, this, &QtWidgetsApplication1::reload);
    connect(Btn1, &QPushButton::clicked, this, &QtWidgetsApplication1::loadaf);
    resize(700, 600);
}

/***********************************静态变量初始化***********************************/
vector<QPointF> QtWidgetsApplication1::citys = vector<QPointF>({QPointF(0, 0)});
int QtWidgetsApplication1::showing = 0;
int QtWidgetsApplication1::base = 10;
vector<vector<int>> QtWidgetsApplication1::routes = vector<vector<int>>();
vector<QPointF> QtWidgetsApplication1::fitnessList = vector<QPointF>({QPointF(0, 0)});

/***********************************训练数据及结果导入***********************************/
void QtWidgetsApplication1::import(vector<pair<double, double>> result_citys, vector<double> result_fitnessList, vector<vector<int>> result_routes) {
    routes = result_routes;
    citys.clear();
    fitnessList.clear();
    for (auto i : result_citys) {
        citys.push_back(QPointF(i.first * 420, i.second * 420));
    }
    base = ceil(result_fitnessList[0] / 10);
    for (int i = 0; i < result_fitnessList.size(); i++) {
        fitnessList.push_back(QPointF(((double)i / (GENE_NUM / 10.0) * 42.0), -(result_fitnessList[i] * 400) / (base * 10))); //坐标缩放转换
    }
}

/***********************************绘图实现***********************************/

void QtWidgetsApplication1::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 启用抗锯齿效果

    painter.translate(30, 100);                               //把原点移到（30，100）位置
    painter.drawRect(-13, -10, Y_MAX + 60, Y_MAX + 50);       //绘出方框
    painter.drawLine(30, 10, 30, Y_MAX + 10);                 //绘制Y轴
    painter.drawLine(30, Y_MAX + 10, 30 + Y_MAX, Y_MAX + 10); //绘制X轴
    painter.translate(30, 430);
    //绘制坐标轴刻度
    for (int i = 0; i * 42 <= Y_MAX; i += 1) {
        if (i * 40 < Y_MAX) {
            painter.drawLine(0, -i * 42, -6, -i * 42);
        }
        painter.drawLine(i * 42, 0, i * 42, 6);
    }

    if (showing > 0) //如果展示的不是适应度曲线
    {

        //绘制坐标值
        for (int i = 0; i * 42 <= Y_MAX; i += 1) {
            if (i * 42 <= Y_MAX && i != 0) {
                painter.drawText(-35, -i * 42 + 5, QString::asprintf("%.2lf", 0.1 * i));
            }
            painter.drawText(i * 42 - 10, 20, QString::asprintf("%.2lf", i * 0.1));
        }
        painter.drawText(300, -450, QString::asprintf("当前最优适应度：%.2lf", -(fitnessList[showing].y() * (base * 10)) / 400));

        /***************绘制路线图****************/
        // drawCity(0,0,painter);
        int af = routes[showing].back(); //前一个点设置成终点，首尾相连；

        for (int &i : routes[showing]) {
            drawCity(citys[i].x(), -citys[i].y(), painter); //画出城市位置
            painter.drawLine(citys[i].x(), -citys[i].y(), citys[af].x(), -citys[af].y());
            af = i;
        }
    } else //展示适应度曲线
    {

        //绘制坐标值
        for (int i = 0; i * 42 <= Y_MAX; i += 1) {
            if (i * 42 <= Y_MAX && i != 0) {
                painter.drawText(-35, -i * 42 + 5, QString::asprintf("%d", i * base));
            }
            painter.drawText(i * 42 - 10, 20, QString::asprintf("%d", i * (GENE_NUM / 10)));
        }

        painter.drawText(300, -450, QString::asprintf("当前最优适应度：%.2lf", -(fitnessList.back().y() * (base * 10)) / 400));
        /***************绘制适应度曲线****************/
        painter.drawPolyline(&fitnessList[0], (int)fitnessList.size()); //由于drawPolyline只接受数组传参，vector在内存中连续存放，所以其起始位置和数组名等效
    }
    // painter.drawPolygon(&citys[0], (int)citys.size());//由于drawPolygon只接受数组传参，vector在内存中连续存放，所以其起始位置和数组名等效
}

void QtWidgetsApplication1::drawCity(double sx, double sy, QPainter &painter) //在城市位置周围画一个小圆突出位置
{
    const int COUNT = 5;   // 边数，越大就越像圆
    QPointF points[COUNT]; // 顶点数组
    double r = CITY_R;
    for (int i = 0; i < COUNT; ++i) {
        float radians = 2 * M_PI / COUNT * i; // M_PI 是 QtMath 里定义的，就是 PI
        float x = sx + r * qCos(radians);
        float y = sy + r * qSin(radians);
        points[i] = QPointF(x, y);
    }
    painter.drawPolygon(points, COUNT);
}

void QtWidgetsApplication1::initList(int n) {
    List->clear();
    List->addItem(QString::asprintf("适应度曲线"));
    // List.setItem(0, 0, &tem);
    for (int i = 1; i < n; i++) {
        QListWidgetItem *item = new QListWidgetItem(QString::asprintf("第%d代最佳个体", i));
        List->addItem(item);
    }
}

void QtWidgetsApplication1::updateMap(QListWidgetItem *item) {
    int column;
    if (item->text() == "适应度曲线") {
        column = 0;
    } else
        sscanf(item->text().toStdString().c_str(), "第%d代最佳个体", &column);
    int a = column;
    showing = column;
    if (column == 0)
        a = fitnessList.size() - 1;
    update();
}

/***********************************训练***********************************/
void QtWidgetsApplication1::reload() {
    //重新训练
    // TODO训练数据
    //随机生成城市图以及距离矩阵
    CityInformation cityinf;
    cityinf.outPutCityList();
    filePath = "cityList.in";
    //用距离矩阵初始化遗传算法类
    Ga ga(cityinf);
    //开始训练
    ga.train();
    vector<pair<double, double>> result_citys = cityinf.cityList; //存储所有城市位置
    vector<double> result_fitnessList = ga.getFitnessList();      //适应度曲线
    vector<vector<int>> result_routes = ga.getResultList();       //存储每一代最佳个体路线

    //更新页面
    import(result_citys, result_fitnessList, result_routes);
    initList((int)routes.size());

    showing = 0;
    update();
}

void QtWidgetsApplication1::loadaf() {
    //重新训练
    // TODO训练数据
    //随机生成城市图以及距离矩阵
    ifstream in(filePath.toStdString(), ios::in);
    CityInformation cityinf(in);
    //用距离矩阵初始化遗传算法类
    Ga ga(cityinf);
    //开始训练
    ga.train();
    vector<pair<double, double>> result_citys = cityinf.cityList; //存储所有城市位置
    vector<double> result_fitnessList = ga.getFitnessList();      //适应度曲线
    vector<vector<int>> result_routes = ga.getResultList();       //存储每一代最佳个体路线

    //更新页面
    import(result_citys, result_fitnessList, result_routes);
    initList((int)routes.size());
    update();
}

void QtWidgetsApplication1::openfile() {
    filePath = QFileDialog::getOpenFileName(this, tr("import data"), "./", tr("data files(*.in);;All files (*.*)"));
    loadaf();
}