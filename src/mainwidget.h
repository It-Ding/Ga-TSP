#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPainter>
#include <vector>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>


#define X_MAX 720
#define Y_MAX 420
#define CITY_R 3

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);//构造
    ~MainWidget();//析构

    void paintEvent(QPaintEvent *);//默认调用的绘画事件
    void import(vector<pair<double,double>> result_citys,vector<double> result_fitnessList,vector<vector<int>> result_routes);//导入数据

    void updateMap(const int &index);//更新展示图像标记,并刷新重新绘制图像

private:
    Ui::MainWidget *ui;

    static vector<QPointF> citys;//存储所有城市位置
    static vector<vector<int>> routes;//存储每一代最佳个体路线
    static vector<QPointF> fitnessList;//适应度曲线
    static int showing;//正在展示的图像标记
    static int base;//适应度图像纵向缩放尺寸

    QComboBox comboBox;//下拉选项卡对象

    void drawCity(double x,double y,QPainter& painter);//在城市位置周围画一个小圆突出位置
    void initComboBox(int n);//初始化拉下选项
    void reload();//重新生成随机数据并开始训练
    void loadaf();//载入上一次训练数据开始训练
};



#endif // MAINWIDGET_H
