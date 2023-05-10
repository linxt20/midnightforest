#ifndef MYCHARACTER_H
#define MYCHARACTER_H

#include "character.h"

class mycharacter : public character
{
public:
    mycharacter(int x=-1,int y=-1,int category = 0,QWidget *parent = nullptr);//构造函数
    ~mycharacter();//析构函数
    bool judgevectory();//判断我方是否胜利
    void judgedead();//判断该单位是否死亡以及死亡后的操作

public:
    bool movebefore =1 ;//判断点击移动按钮后是否进行了移动操作
    bool fightbefore = 1 ;//判断点击攻击按钮后是否进行了攻击操作


};

#endif // MYCHARACTER_H
