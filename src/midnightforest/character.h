#ifndef CHARACTER_H
#define CHARACTER_H


#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <QObject>
#include <QPropertyAnimation>

class character : public QLabel
{
public:
    explicit character(QWidget *parent = nullptr);//构造函数
    ~character();//析构函数
    QString name;//名字
    int x,y;//坐标
    int level;//等级
    int HP;//生命值
    int aggressivity;//攻击力
    int defensivepower;//防御力
    int Movingrange;//移动范围
    int fightrange;//进攻范围
    QString imageaddress;//图片地址
    QString gifaddress;//技能gif地址

    int movecishu = 1;//剩余移动次数
    int fightcishu = 1 ;//剩余攻击次数

    bool isStandby = 0;//是否待机
    bool isdead = 0 ;//是否死亡

public:
    virtual void judgedead()=0;//死亡判断函数 虚函数

};

#endif // CHARACTER_H
