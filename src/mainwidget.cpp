#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "Ga.h"
#include <QTest>


/***********************************MainWidget构造***********************************/
MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent),ui(new Ui::MainWidget)
    , comboBox(this)
{
    ui->setupUi(this);
    QPushButton *Btn=new QPushButton("生成数据并训练",this);
    QPushButton *Btn1=new QPushButton("重新训练",this);
    Btn->move(300,30);
    Btn1->move(500,30);
    connect(Btn,&QPushButton::clicked,this,&MainWidget::reload);
    connect(Btn1,&QPushButton::clicked,this,&MainWidget::loadaf);
    initComboBox((int)routes.size());
    connect(&comboBox,&QComboBox::currentIndexChanged,this,&MainWidget::updateMap);//链接下拉框返回信息和绘制图像的函数，实现更改下拉框选项就更改图像
    resize(800,600);

}

MainWidget::~MainWidget()
{
    delete ui;
}

/***********************************静态变量初始化***********************************/
vector<QPointF> MainWidget::  citys =vector<QPointF>({QPointF(0,0)});
int MainWidget:: showing=0;
int MainWidget:: base=10;
vector<vector<int>>MainWidget:: routes=vector<vector<int>>();
vector<QPointF> MainWidget:: fitnessList=vector<QPointF>({QPointF(0,0)});


/***********************************训练数据及结果导入***********************************/
void MainWidget::import(vector<pair<double,double>> result_citys,vector<double> result_fitnessList,vector<vector<int>> result_routes)
{
    routes=result_routes;
    citys.clear();
    fitnessList.clear();
    for(auto i:result_citys)
    {
        citys.push_back(QPointF(i.first*720,i.second*420));
    }
    base=ceil(result_fitnessList[0]/10);
    for(int i=0;i<result_fitnessList.size();i++)
    {
        fitnessList.push_back(QPointF(((double)i/(GENE_NUM/20.0)*36.0),(result_fitnessList[i]*400)/(base*10)));//坐标缩放转换
    }
}
\


/***********************************绘图实现***********************************/

void MainWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);// 启用抗锯齿效果

    painter.translate(30, 100);//把原点移到（30，100）位置
    painter.drawRect(-13, -10,X_MAX+60, Y_MAX+50);//绘出方框
    painter.drawLine(30,10,30,Y_MAX+10);//绘制Y轴
    painter.drawLine(30,Y_MAX+10,30+X_MAX,Y_MAX+10);//绘制X轴

    if(showing>0)//如果展示的不是适应度曲线
    {
        /***************完善坐标系****************/
        //转化为数学坐标系，原点在左下角
        painter.setWindow(0,height(),width(),-height());
        painter.translate(30,-30);
        //绘制坐标轴刻度
        for(int i=0;i<=X_MAX;i+=1)
        {
            if(i*40<Y_MAX)
            {
                painter.drawLine(0,i*40,-6,i*40);
            }
            painter.drawLine(i*40,0,i*40,-6);
        }
        //恢复默认坐标系，原点在左上角
        painter.setWindow(0, 0, this->width(), this->height());
        painter.translate(0,40+Y_MAX);
        //绘制坐标值
        for(int i=0;i<=X_MAX;i+=1)
        {
            if(i*40<Y_MAX&&i!=0)
            {
                painter.drawText(-35, -i*40+5, QString::asprintf("%d",(i)*40));
            }
            painter.drawText(i*40-10,20, QString::asprintf("%d",i*40));
        }
        /////////////////////////////////////////////
        painter.drawText(600,-450, QString::asprintf("当前最优适应度：%.2lf",(fitnessList[showing].y()*(base*10))/400));
        painter.setWindow(0,height(),width(),-height());
        painter.translate(0,-40-Y_MAX);
        /***************绘制路线图****************/
        //drawCity(0,0,painter);
        int af=routes[showing].back();//前一个点设置成终点，首尾相连；
        for(int& i:routes[showing])
        {
            drawCity(citys[i].x(),citys[i].y(),painter);//画出城市位置
            painter.drawLine(citys[i].x(),citys[i].y(),citys[af].x(),citys[af].y());
            af=i;
        }
    }
    else//展示适应度曲线
    {
        /***************完善坐标系****************/
        //转化为数学坐标系，原点在左下角
        painter.setWindow(0,height(),width(),-height());
        painter.translate(30,-30);
        //绘制坐标轴刻度
        for(int i=0;i<=X_MAX;i+=1)
        {
            if(i*40<Y_MAX)
            {
                painter.drawLine(0,i*40,-6,i*40);
            }
            painter.drawLine(i*36,0,i*36,-6);
        }
        //恢复默认坐标系，原点在左上角
        painter.setWindow(0, 0, this->width(), this->height());
        painter.translate(0,40+Y_MAX);
        //绘制坐标值
        for(int i=0;i<=X_MAX;i+=1)
        {
            if(i*40<Y_MAX&&i!=0)
            {
                painter.drawText(-35, -i*40+5, QString::asprintf("%d",i*base));
            }
            painter.drawText(i*36-10,20, QString::asprintf("%d",i*(GENE_NUM/20)));
        }

        painter.drawText(600,-450, QString::asprintf("当前最优适应度：%.2lf",(fitnessList.back().y()*(base*10))/400));
        painter.setWindow(0,height(),width(),-height());
        painter.translate(0,-40-Y_MAX);
        //drawCity(0,0,painter);
        /***************绘制适应度曲线****************/
        painter.drawPolyline(&fitnessList[0], (int)fitnessList.size());//由于drawPolyline只接受数组传参，vector在内存中连续存放，所以其起始位置和数组名等效

    }
    //painter.drawPolygon(&citys[0], (int)citys.size());//由于drawPolygon只接受数组传参，vector在内存中连续存放，所以其起始位置和数组名等效
}


void MainWidget::drawCity(double sx,double sy,QPainter& painter)//在城市位置周围画一个小圆突出位置
{
    const int COUNT  = 5;  // 边数，越大就越像圆
    QPointF points[COUNT];  // 顶点数组
    double r=CITY_R;
    for (int i = 0; i < COUNT; ++i)
    {
        float radians = 2 * M_PI / COUNT * i; // M_PI 是 QtMath 里定义的，就是 PI
        float x = sx+r * qCos(radians);
        float y = sy+r * qSin(radians);
        points[i] = QPointF(x, y);
    }
    painter.drawPolygon(points, COUNT);
}


void MainWidget::initComboBox(int n)//初始化下拉选项
{
    //"初始化列表"按键
    comboBox.move(30,30);
    comboBox.clear(); //清除列表
    comboBox.addItem(QString::asprintf("    适应度曲线      "));
    for (int i=1;i<n;i++)
    {
        comboBox.addItem(QString::asprintf("第%d代最佳个体",i));
    }
}

void MainWidget::updateMap(const int &index)
{
    int a=index;
    showing=index;
    if(index==0)a=fitnessList.size()-1;
    update();
}

/***********************************训练***********************************/
void MainWidget::reload()
{
    //重新训练
    //TODO训练数据
    //随机生成城市图以及距离矩阵
    CityInformation cityinf;
    cityinf.outPutCityList();
    //用距离矩阵初始化遗传算法类
    Ga ga(cityinf);
    //开始训练
    ga.train();
    vector<pair<double,double>> result_citys=cityinf.cityList;//存储所有城市位置
    vector<double> result_fitnessList=ga.getFitnessList();//适应度曲线
    vector<vector<int>> result_routes=ga.getResultList();//存储每一代最佳个体路线

//    result_citys=vector<pair<int,int>>({{30,30},{45,68},{45,35},{86,37}});
//    result_fitnessList=vector<pair<int,int>>({{1,300},{30,200},{45,35},{140,30}});

//    result_routes.push_back(vector<int>());
//    result_routes.push_back(vector<int>({1,2,3,0}));
//    result_routes.push_back(vector<int>({1,3,2,0}));
//    result_routes.push_back(vector<int>({0,1,3,2}));

    //更新页面
    import(result_citys,result_fitnessList,result_routes);
    initComboBox((int)routes.size());
    update();
}

void MainWidget::loadaf()
{
    //重新训练
    //TODO训练数据
    //随机生成城市图以及距离矩阵
    ifstream in("cityList.in",ios::in);
    CityInformation cityinf(in);
    //用距离矩阵初始化遗传算法类
    Ga ga(cityinf);
    //开始训练
    ga.train();
    vector<pair<double,double>> result_citys=cityinf.cityList;//存储所有城市位置
    vector<double> result_fitnessList=ga.getFitnessList();//适应度曲线
    vector<vector<int>> result_routes=ga.getResultList();//存储每一代最佳个体路线

//    result_citys=vector<pair<int,int>>({{30,30},{45,68},{45,35},{86,37}});
//    result_fitnessList=vector<pair<int,int>>({{1,300},{30,200},{45,35},{140,30}});

//    result_routes.push_back(vector<int>());
//    result_routes.push_back(vector<int>({1,2,3,0}));
//    result_routes.push_back(vector<int>({1,3,2,0}));
//    result_routes.push_back(vector<int>({0,1,3,2}));

    //更新页面
    import(result_citys,result_fitnessList,result_routes);
    initComboBox((int)routes.size());
    update();
}
