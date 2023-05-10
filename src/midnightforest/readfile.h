#ifndef READFILE_H
#define READFILE_H

#include <QObject>
#include <QFile>

enum MapElement//地形信息枚举
{
    grass,//0绿色草地
    river,//1河流
    wall,//2城墙
    trap,//3草地陷阱
    stone,//4石块
    bartizan,//5箭塔
    granary,//6粮仓
    bridge,//7桥梁
    redgrass,//8红色地板
    redstone,//9红色石头
    redtrap,//10火神柱陷阱
    whitegrass,//11白色地板
    whitestone,//12冰晶石头
    whitetrap,//13冰湖陷阱
    blackgrass,//14黑色地板
    blackstone,//15青铜石头
    blacktrap,//16地刺陷阱
};

class readfile:public QObject
{
    Q_OBJECT
public:
    explicit readfile(QObject *parent = nullptr);//构造函数
    ~readfile();//析构函数
    void initbyfile(QString filename);//文件内容分解函数
    void Clear();//delete数组

    int mrow;//地形行
    int mcol;//地形列
    int** imagemap;//地图二维数组

public slots:

};

#endif // READFILE_H
