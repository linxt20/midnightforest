#include "mycharacter.h"
#include "mainwindow.h"

extern int mysubmap[15][22];//小心初始化
extern int enemysubmap[15][22];
extern int upgrade;
extern int section;


mycharacter::mycharacter(int _x,int _y,int _category,QWidget *parent):character(parent)
{
    x=_x;//人物坐标
    y=_y;
    switch(_category)//分类录入信息
    {
    case 1:
        name = "swordsman";//姓名
        level = 1+upgrade;//等级
        HP = 90+20*upgrade;//生命值
        aggressivity=55+5*upgrade;//攻击力
        defensivepower=20+5*upgrade;//防御力
        Movingrange = 2;//移动范围
        fightrange = 1;//进攻范围
        imageaddress = ":/myrole/allpicture/mymelee.png";//图片地址
        gifaddress = ":/my/allgif/mymelee.gif";//技能动图地址
        break;
    case 2:
        name = "Spearthrower";//姓名
        level = 1+upgrade;//等级
        HP = 90+20*upgrade;//生命值
        aggressivity=50+5*upgrade;//攻击力
        defensivepower=20+5*upgrade;//防御力
        Movingrange = 2;//移动范围
        fightrange = 2;//进攻范围
        imageaddress = ":/myrole/allpicture/myArcher.png";//图片地址
        gifaddress = ":/my/allgif/myArcher.gif";//技能动图地址
        break;
    case 3:
        name = "Frost Knight";//姓名
        level = 1+upgrade;//等级
        HP = 120+20*upgrade;//生命值
        aggressivity=65+5*upgrade;//攻击力
        defensivepower=25+5*upgrade;//防御力
        Movingrange = 3;//移动范围
        fightrange = 1;//进攻范围
        imageaddress = ":/myrole/allpicture/myKnight.png";//图片地址
        gifaddress = ":/my/allgif/myKnight.gif";//技能动图地址
        break;
    case 4:
        name = "Shura mage";//姓名
        level = 1+upgrade;//等级
        HP = 100+20*upgrade;//生命值
        aggressivity=75+5*upgrade;//攻击力
        defensivepower=15+10*upgrade;//防御力
        Movingrange = 2;//移动范围
        fightrange = 2;//进攻范围
        imageaddress = ":/myrole/allpicture/mymage.png";//图片地址
        gifaddress = ":/my/allgif/mymage.gif";//技能动图地址
        break;
    case 5:
        name = "Shieldbearer" ;//姓名
        level = 1+upgrade;//等级
        HP = 180+30*upgrade;//生命值
        aggressivity=50+5*upgrade;//攻击力
        defensivepower=35+5*upgrade;//防御力
        Movingrange = 2;//移动范围
        fightrange = 1;//进攻范围
        imageaddress = ":/myrole/allpicture/myshield.png";//图片地址
        gifaddress = ":/my/allgif/myshield.gif";//技能动图地址
        break;
    case 6:
        name = "Guan Yu";//姓名
        level = 1+upgrade;//等级
        HP = 400+50*upgrade;//生命值
        aggressivity=75+10*upgrade;//攻击力
        defensivepower=35+10*upgrade;//防御力
        Movingrange = 2;//移动范围
        fightrange = 1;//进攻范围
        imageaddress = ":/myrole/allpicture/myboss.png";//图片地址
        gifaddress = ":/my/allgif/mybossgif.gif";//技能动图地址
        break;
    }
}

mycharacter::~mycharacter()//析构函数
{

}

bool mycharacter::judgevectory()//我方胜利判断 在第四关，敌方boss诸葛亮死亡则胜利，一到三关则是消灭全部敌人
{
    if(section == 4)
    {
        for(int i=0;i<15;i++)
        {
            for(int j=0;j<22;j++)
            {
                if(enemysubmap[i][j]==0)//诸葛亮占位最后是第一个被识别到的，一定是0号
                   return false;
            }
        }
    }
    else{
        for(int i=0;i<15;i++)
        {
            for(int j=0;j<22;j++)
            {
                if(enemysubmap[i][j]>=0)//判断是否存在敌人
                   return false;
            }
        }
    }
    return true;
}

void mycharacter::judgedead()//判断该战士是否死亡，以及死亡后隐藏
{
    if(HP<=0)
    {
        HP = 0;
        hide();
        mysubmap[x][y]=-1;
        isdead = 1;
    }
}


