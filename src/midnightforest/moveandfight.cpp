#include "mainwindow.h"
#include "ui_mainwindow.h"

extern int needle;//3秒定时器指针
extern int mysubmap[15][22];//我方战士地图
extern int enemysubmap[15][22];//敌方战士地图
extern int hang,lie;//点击点的行列
extern int mypeople;//我方战士人数
int searchpath[15][22];//最短路径路径搜索地图

//我方移动动画函数
void MainWindow::mymoveanimation(int i, int j)
{
    QPropertyAnimation *myanimation = new QPropertyAnimation(warrior[mysubmap[hang][lie]], "geometry");
    myanimation->setDuration(2000);
    myanimation->setStartValue(QRect(lie*50,hang*50,50,50));
    myanimation->setEndValue(QRect(j*50,i*50,50,50));
    myanimation->start();
    connect(myanimation, &QPropertyAnimation::finished, this,[this,i,j]() {
        mymovefinish(i,j);//移动动画结束使用lamda表达式实现后续操作
    });
}
//敌方移动动画函数
void MainWindow::enemymoveanimation(int i, int num)
{
    QPropertyAnimation *enemyanimation = new QPropertyAnimation(enemywarrior[i], "geometry");
    enemyanimation->setDuration(2000);
    enemyanimation->setStartValue(QRect(enemywarrior[i]->y*50,enemywarrior[i]->x*50,50,50));
    enemyanimation->setEndValue(QRect(enemywarrior[i]->pathy[num]*50,enemywarrior[i]->pathx[num]*50,50,50));
    enemyanimation->start();
    connect(enemyanimation, &QPropertyAnimation::finished,this,[this,i,num]() {
        enemymovefinish(i,num);
    });
    needle = 0;
    time->start(1000);
}

//我方移动动画结束 进行信息传递，两个位置信息的转化
void MainWindow::mymovefinish(int i, int j)
{
    warrior[mysubmap[hang][lie]]->movebefore = 0 ;
    this->movebutton();//调用移动函数，消除移动标记
    mysubmap[i][j] = mysubmap[hang][lie];
    mysubmap[hang][lie] = -1 ;
    hang = i;//标记当前点击位置的行与列
    lie = j ;
    warrior[mysubmap[i][j]]->x=hang;
    warrior[mysubmap[i][j]]->y=lie;
}
//敌方移动动画结束 进行信息传递，两个位置信息的转化
void MainWindow::enemymovefinish(int i, int num)
{
    enemysubmap[enemywarrior[i]->pathx[num]][enemywarrior[i]->pathy[num]] = i ;
    enemysubmap[enemywarrior[i]->x][enemywarrior[i]->y] = -1;
    enemywarrior[i]->x=enemywarrior[i]->pathx[num];
    enemywarrior[i]->y=enemywarrior[i]->pathy[num];
}


//双方的战斗动画
void MainWindow::fightanimation(character *person, int hpchange)
{
    movie = new QMovie(person->gifaddress);
    movie->setSpeed(5000);//动画加速
    labelfightbackground->show();//动画背景板
    labelfightbackground->raise();
    labelHPchangetext->show();//伤害结算板
    labelHPchangetext->raise();
    lcdHPchangenum->display(hpchange);//伤害显示板
    lcdHPchangenum->show();
    lcdHPchangenum->raise();
    labelfightskill->setMovie(movie);//动画板
    labelfightskill->show();
    labelfightskill->raise();
    fightingmusic(person->name);//音效板块
    movie->start();
    needle = 0;
    time->start(1000);
}

void MainWindow::fightbuttonshow(int i)//敌方ai战斗 进攻方与被攻击方都需要显示红色区别出来
{
    button[enemywarrior[i]->x][enemywarrior[i]->y]->setFlat(false);
    button[enemywarrior[i]->x][enemywarrior[i]->y]->setStyleSheet("background-color:rgba(255,0,0,0.5)");
    button[warrior[enemywarrior[i]->opponent]->x][warrior[enemywarrior[i]->opponent]->y]->setFlat(false);
    button[warrior[enemywarrior[i]->opponent]->x][warrior[enemywarrior[i]->opponent]->y]->setStyleSheet("background-color:rgba(255,0,0,0.5)");
}

void MainWindow::fightbuttonhide(int i)//敌方ai战斗结束 进攻方与被攻击方显示的红色消失
{
    button[enemywarrior[i]->x][enemywarrior[i]->y]->setFlat(true);
    button[enemywarrior[i]->x][enemywarrior[i]->y]->setStyleSheet("background:transparent;");
    button[warrior[enemywarrior[i]->opponent]->x][warrior[enemywarrior[i]->opponent]->y]->setFlat(true);
    button[warrior[enemywarrior[i]->opponent]->x][warrior[enemywarrior[i]->opponent]->y]->setStyleSheet("background:transparent;");
}

void MainWindow::findnearperson(int i)//寻找最近的我方战士
{
    enemywarrior[i]->minstep=100;//设置一个较大的初始值
    for(int t = 0 ;t<mypeople;t++)//我方战士轮换
    {
        if(!warrior[t]->isdead)
        {
            for(int j=0 ;j<15;j++)//赋值阻碍地图，阻碍型地形以及敌方战士均是阻碍
            {
                for(int k=0;k<22;k++)
                {
                    searchpath[j][k] = 0;
                    if(mpmap->imagemap[j][k] == 1||mpmap->imagemap[j][k] == 2||mpmap->imagemap[j][k] == 4||mpmap->imagemap[j][k]==9 ||mpmap->imagemap[j][k]==12 ||mpmap->imagemap[j][k]==15|| enemysubmap[j][k]>=0)
                    {
                        searchpath[j][k]= -1 ;
                    }
                }
            }
            enemywarrior[i]->find(warrior[t]->x,warrior[t]->y);//寻找最近的路径
        }
    }
}

void MainWindow::fightingmusic(QString  type)//战斗音效 总共三种 诸葛亮 关羽 以及所有的小兵 三种战斗音效
{
    QString file;
    if(type == "Guan Yu")
    {
        file= ":/fignting/allwav/fire.wav";
    }
    else{
        if(type != "Zhuge Liang")
        {
            file= ":/fignting/allwav/sword.wav";
        }
        else{
            file= ":/fignting/allwav/ligntning.wav";
        }
    }
    fightingm_pSoundEffect = new QSoundEffect(this);//战斗音效开始
    fightingm_pSoundEffect->setSource(QUrl::fromLocalFile(file));
    fightingm_pSoundEffect->setLoopCount(QSoundEffect::Infinite);
    fightingm_pSoundEffect->setVolume(1.0f);
    fightingm_pSoundEffect->play();
}
