#ifndef ENEMYCHARACTER_H
#define ENEMYCHARACTER_H

#include <QLabel>
#include "character.h"
#include "mycharacter.h"
#include <stack>
//#include <iostream>
using namespace std;

struct Postion//结构体储存路径上的点
{
    int X, Y;
    Postion(){}//构造函数
    Postion(int X, int Y): X(X), Y(Y){}
};

class enemycharacter : public character//敌人类
{
public:
    enemycharacter(int x=-1,int y=-1,int category = 0,QWidget *parent = nullptr);//构造函数
    bool judgefightrange();//判断攻击范围内是否有我方战士
    int movepoint();//判断移动目标点

    void find(int pointx,int pointy);//寻找我方战士的深搜起始函数
    void search(stack<Postion> &path,int row,int col);//深搜进行函数
    void printPath(stack<Postion> MinPATH);//储存与传递最短路径在行动范围内的一段
    bool canplace(int prePosValue,int row,int col);//判断当前点能否走通
    bool judgefalse();//判断我方是否失败
    void judgedead();//该单位是否死亡以及死亡后的操作

public:
    stack<Postion> MinPATH;// 记录最短路径
    int pathx[3];//行走路径横坐标
    int pathy[3];//行走路径纵坐标
    int opponent;//攻击范围内我方战士的序号
    int minstep;//最少的步数
};

#endif // ENEMYCHARACTER_H
