#include "mainwindow.h"

#include <QApplication>

int section = 1;//关卡
int upgrade = 0;//通关次数
int mysubmap[15][22];//我方战士地图
int enemysubmap[15][22];//敌方战士地图
int movefightmap[15][22];//攻击与移动点击范围的限制
int hang=-1,lie=-1;//点击点的行列

int main(int argc, char *argv[])
{
    for(int i=0;i<15;i++)//初始化
    {
        for(int j=0;j<22;j++)
        {
            mysubmap[i][j]=-1;
            enemysubmap[i][j]=-1;
            movefightmap[i][j]=-1;
        }
    }
    QApplication a(argc, argv);
    MainWindow w;//总窗口
    w.show();
    return a.exec();
}
