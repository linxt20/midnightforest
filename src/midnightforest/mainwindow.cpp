#include "mainwindow.h"
#include "ui_mainwindow.h"

int needle = 0;//3秒定时器指针
int enemyroletimeneedle = 0;//敌人出场指针
int delayedneedle =0;//敌人动画线程堵塞处指针
int huihe = 0;//现在的回合情况 0表示放置回合 1表示我方回合 2表示敌方回合
int nowchose = 0 ;//放置回合时现在选中的战士的种类号
int mypeople = 0;//我方战士人数
int enemypeople = 0 ;//敌方战士人数
extern int mysubmap[15][22];//我方战士地图
extern int enemysubmap[15][22];//敌方战士地图
extern int movefightmap[15][22];//攻击与移动点击范围的限制
extern int section;//关卡
extern int hang,lie;//点击点的行列
extern int upgrade;//通关次数
QString state;//当前状态 分为三种情况： 空（点击地图可以显示地形人物信息），移动（点击地图可以移动），攻击（点击地图可以攻击）

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    uibaseoperator();//部分ui界面设置的函数

    QString file = ":/else/allwav/start.wav";//开始游戏界面的音乐
    backgroundonplay(file);

    time = new QTimer(this);
    connect(time,SIGNAL(timeout()),this,SLOT(timeoperator()));//3秒定时器链接

    enemyroletime = new QTimer(this);
    connect(enemyroletime,SIGNAL(timeout()),this,SLOT(enemyroletimeoperator()));//敌人显示定时器链接

    delayedtime = new QTimer(this);
    connect(delayedtime,SIGNAL(timeout()),this,SLOT(delayedtimeoperator()));//线程堵塞定时器链接

    connect(ui->information,&QPushButton::clicked,this,[this](){//战斗右侧信息栏的信息按钮与信息界面的链接
        ui->stackedWidget_2->setCurrentIndex(4);
    });

    connect(ui->rolejinzhan,&QPushButton::clicked,this,[this](){//放置回合，放置战士链接
        same_operator_in_place(1,ui->lcdNumberjinzhan);
    });
    connect(ui->rolesheshou,&QPushButton::clicked,this,[this](){//放置回合，放置射手链接
        same_operator_in_place(2,ui->lcdNumbersheshou);
    });
    connect(ui->roleqishi,&QPushButton::clicked,this,[this](){//放置回合，放置骑士链接
        same_operator_in_place(3,ui->lcdNumberqishi);
    });
    connect(ui->rolefashi,&QPushButton::clicked,this,[this](){//放置回合，放置法师链接
        same_operator_in_place(4,ui->lcdNumberfashi);
    });
    connect(ui->roledunzhan,&QPushButton::clicked,this,[this](){//放置回合，放置盾战链接
        same_operator_in_place(5,ui->lcdNumberdunzhan);
    });
    connect(ui->roleboss,&QPushButton::clicked,this,[this](){//放置回合，放置boss链接
        same_operator_in_place(6,ui->lcdNumberboss);
    });

    connect(ui->move,SIGNAL(clicked()),this,SLOT(movebutton()));//战斗信息栏的移动按钮链接
    connect(ui->fight,SIGNAL(clicked()),this,SLOT(fightbutton()));//战斗信息栏的攻击按钮链接
    connect(ui->roleover,SIGNAL(clicked()),this,SLOT(roleoverbutton()));//战斗信息栏的待机按钮链接
    connect(ui->withdraw,SIGNAL(clicked()),this,SLOT( withdrawbutton()));//战斗信息栏的撤退按钮链接

    connect(ui->vectorycontinue,&QPushButton::clicked,this,[this](){//胜利按钮链接 使用lamda表达式 设置通关第四关会有大结局的剧情动画
        if(section == 4)//大结局剧情动画
        {
            m_pSoundEffect->stop();
            ui->stackedWidget->setCurrentIndex(5);
            potmovie = new QMovie(":/pot/allgif/finally.gif");
            potmovie->setSpeed(105);
            ui->label_6->setScaledContents(true);
            ui->label_6->setMovie(potmovie);
            potmovie->start();
            QString file = ":/pot/allwav/finally.wav";
            backgroundonplay(file);
        }
        else{//普通胜利，返回选关界面
            QString file = ":/else/allwav/select.wav";
            same_operator_in_continue(file);
        }
    });
    connect(ui->falsecontinue,&QPushButton::clicked,this,[this](){//失败，返回玄关界面
        QString file= ":/else/allwav/select.wav";
        same_operator_in_continue(file);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)//事件过滤器——绘画事件
{
    if(event->type() == QEvent::Paint)
    {
        showPaint(watched); //响应函数
    }
    return QWidget::eventFilter(watched,event);
}

//该函数的主要逻辑为：当点击一个地图上的方格后，
//1.判断现在是什么回合，如果是我方回合，转1.1，如果是放置回合转1.2
//1.1 判断此时的点击状态 如果状态为空 转1.1.1如果点过移动，转1.1.2，如果点过战斗，转1.1.3
//1.1.1 根据地形地图、我方战士地图、地方展示地图，返回对应的信息介绍
//1.1.2 判断是否movefightmap标记的移动范围，是则启动移动动画，否则点了个寂寞
//1.1.3 判断是否movefightmap标记的进攻范围，是则启动攻击动画，实现伤害结算，判断我方是否取得胜利
//1.2 判断此时的点击状态nowchoose，如果nowchoose！=0表示点过个人，则放置，如果nowchoose==0则表示啥都没点，则点了个寂寞
void MainWindow::clickedjudge(int i,int j)//点击判断
{
    if(huihe)
    {
        if(state.isEmpty())
        {
            if(mysubmap[i][j]== -1 && enemysubmap[i][j] == -1)//信息窗口跳转判断，如果当前就是这页就不跳了
            {
                if(ui->stackedWidget_2->currentIndex()!=2)
                {
                    ui->stackedWidget_2->setCurrentIndex(2);
                }
                textmapping(i,j);//根据点击的地图信息，在右侧的信息栏显示地形介绍
            }
            else{
                if(mysubmap[i][j]>=0)
                {
                    if(ui->stackedWidget_2->currentIndex()!=3)
                    {
                        ui->stackedWidget_2->setCurrentIndex(3);
                    }
                    informationshow(warrior[mysubmap[i][j]]);//人物信息展示
                }
                if(enemysubmap[i][j]>=0)
                {
                    if(ui->stackedWidget_2->currentIndex()!=4)
                    {
                        ui->stackedWidget_2->setCurrentIndex(4);
                    }
                    informationshow(enemywarrior[enemysubmap[i][j]]);//任务信息展示
                }
            }
        }
        else
        {
            if(movefightmap[i][j] ==1)//判断是否为战斗或者移动范围
            {
                if(state == "move")//移动状态
                {
                    mymoveanimation(i,j);//启动移动动画
                }
                else{//战斗状态
                    if(enemysubmap[i][j]== -1)//打队友是不被允许的
                    {
                        return;
                    }
                    //伤害结算
                    int hpchange = warrior[mysubmap[hang][lie]]->aggressivity-enemywarrior[enemysubmap[i][j]]->defensivepower;
                    enemywarrior[enemysubmap[i][j]]->HP =  enemywarrior[enemysubmap[i][j]]->HP - hpchange;
                    enemywarrior[enemysubmap[i][j]]->judgedead();
                    //启动战斗动画
                    fightanimation(warrior[mysubmap[hang][lie]], hpchange);
                    delayedneedle = 0;//阻塞计时器
                    delayedtime->start(800);
                    loop.exec();
                    fightingm_pSoundEffect->stop();//背景音乐关闭
                    warrior[mysubmap[hang][lie]]->fightbefore = 0;
                    this->fightbutton();
                    if(warrior[mysubmap[hang][lie]]->judgevectory())//判断我方是否胜利
                    {
                        delayedneedle = 0;//阻塞计时器
                        delayedtime->start(200);
                        loop.exec();
                        if(upgrade<8)//胜利通过，则闯关数加1，等级加1，相应数据也会提升
                        {
                            upgrade++;
                        }
                        mpmap->Clear();//数据清理
                        enemyrolemap->Clear();//数据清理

                        for(int i=0;i<mypeople;i++)//剩余人物隐藏
                        {
                            warrior[i]->hide();
                        }
                        m_pSoundEffect->stop();
                        ui->stackedWidget->setCurrentIndex(3);//跳转胜利窗口
                        QString file = ":/else/allwav/vectory.wav";
                        backgroundonplay(file);
                    }
                }
            }
        }
    }
    else
    {
        if(nowchose == 0)
        {
            return;
        }
        else{//障碍判断，不能放置在我方敌方以及阻碍型地形上
             if(mpmap->imagemap[i][j] == 1 || mpmap->imagemap[i][j] == 2 ||mpmap->imagemap[i][j]==4||mpmap->imagemap[i][j]==9||mpmap->imagemap[i][j]==12||mpmap->imagemap[i][j]==15|| enemysubmap[i][j]>=0 || mysubmap[i][j]>=0)
                 return;
            mysubmap[i][j] = mypeople;
            warrior[mypeople] = new mycharacter(i,j,nowchose,ui->firstsection);
            drawhero(i,j,warrior[mypeople]);//画我方展示
            mypeople++;//我方战士人数
            nowchose = 0;
        }
    }
}

//该函数的思路为：（进攻时需要判断箭塔加成，并且敌方战士如果在箭塔中则不考虑移动）
//1.敌方回合开始实现我方特殊地形伤害以及回血的结算
//2.循环轮换敌方战士一个一个实现操作
//3.每个敌人战士先判断周围是否有我方战士，有转3.1，没有则转3.2
//3.1 先进攻，判断我方战士是否死亡，死亡转3.1.1，没死亡则不动
//3.1.1 进行我方失败判断（我方战士死完了没），如果有则直接判定我方失败，没有则转3.1.1.1
//3.1.1.1 轮换我方战士进行深搜寻找距离最短的单位以及之间的最短路径，开启移动动画
//3.2 轮换我方战士进行深搜寻找距离最短的单位以及之间的最短路径，
//开启移动动画，判断周围是否有我方战士，没有则不动，有则转3.2.1
//3.2.1 进攻，进行我方失败判断，后不动
void MainWindow::enemyhuihe()//敌方ai设计
{
    for(int i=0;i<mypeople;i++)//特殊地形对我方的伤害结算
    {
        if(!warrior[i]->isdead)
        {
            switch(mpmap->imagemap[warrior[i]->x][warrior[i]->y])
            {
            case 6:warrior[i]->HP = warrior[i]->HP +50;break;
            case 3:warrior[i]->HP = warrior[i]->HP - 30;break;
            case 10:warrior[i]->HP = warrior[i]->HP - 40;break;
            case 13:warrior[i]->HP = warrior[i]->HP - 50;break;
            case 16:warrior[i]->HP = warrior[i]->HP - 60;break;
            }
            warrior[i]->judgedead();
        }
    }
    for(int i=0;i< enemypeople ;i++)//敌方战士轮换出战
    {
        if(!enemywarrior[i]->isdead)//先判断整个战士是死是活
        {
            if(mpmap->imagemap[enemywarrior[i]->x][enemywarrior[i]->y]==5)//判断箭塔加成
            {
                enemywarrior[i]->aggressivity+=10;
                enemywarrior[i]->defensivepower+=10;
                enemywarrior[i]->fightrange+=1;
            }
            if(enemywarrior[i]->judgefightrange())//判断周围是否有我方战士
            {
                fightbuttonshow(i);//战斗双方地上显示颜色
                int hpchange = enemywarrior[i]->aggressivity-warrior[enemywarrior[i]->opponent]->defensivepower;
                warrior[enemywarrior[i]->opponent]->HP =  warrior[enemywarrior[i]->opponent]->HP - hpchange;
                warrior[enemywarrior[i]->opponent]->judgedead();
                fightanimation(enemywarrior[i], hpchange);//战斗动画
                delayedneedle = 0;
                delayedtime->start(1000);//点睛之笔，动画还未结束，为防止线程继续运行，设置线程堵塞，有效保障动画质量
                loop.exec();
                fightingm_pSoundEffect->stop();
                fightbuttonhide(i);//战斗结束地块颜色消失
                if(mpmap->imagemap[enemywarrior[i]->x][enemywarrior[i]->y]==5)//战斗结束箭塔加成消失
                {
                    enemywarrior[i]->aggressivity-=10;
                    enemywarrior[i]->defensivepower-=10;
                    enemywarrior[i]->fightrange-=1;
                }
                if(warrior[enemywarrior[i]->opponent]->isdead)//判断我方战士是否死亡
                {
                    if(enemywarrior[i]->judgefalse())//判断我方战士是否全都死亡
                    {
                        for(int i=0;i<enemypeople;i++)
                        {
                            enemywarrior[i]->hide();//人物隐藏
                        }
                        mpmap->Clear();//清理数据
                        enemyrolemap->Clear();//清理数据
                        m_pSoundEffect->stop();
                        ui->stackedWidget->setCurrentIndex(4);//跳转失败窗口
                        QString file = ":/else/allwav/false.wav";
                        backgroundonplay(file);
                        return ;
                    }
                    if(mpmap->imagemap[enemywarrior[i]->x][enemywarrior[i]->y]!=5)//如果不在箭塔中
                    {
                        findnearperson(i);//寻找我方战士
                        int num = enemywarrior[i]->movepoint();
                        enemymoveanimation(i,num);//移动动画
                        delayedneedle = 0;
                        delayedtime->start(1000);
                        loop.exec();
                    }
                }
            }
            else
            {
                if(mpmap->imagemap[enemywarrior[i]->x][enemywarrior[i]->y]==5)//如果在箭塔中则不动
                {
                    enemywarrior[i]->aggressivity-=10;
                    enemywarrior[i]->defensivepower-=10;
                    enemywarrior[i]->fightrange-=1;
                }
                if(mpmap->imagemap[enemywarrior[i]->x][enemywarrior[i]->y]!=5)//不在箭塔中则动
                {
                    findnearperson(i);//寻找我方战士
                    int num = enemywarrior[i]->movepoint();
                    enemymoveanimation(i,num);//移动动画
                    delayedneedle = 0;
                    delayedtime->start(1000);
                    loop.exec();
                    if(enemywarrior[i]->judgefightrange())//判断攻击范围内是否有我方战士
                    {
                        fightbuttonshow(i);
                        //伤害结算
                        int hpchange = enemywarrior[i]->aggressivity-warrior[enemywarrior[i]->opponent]->defensivepower;
                        warrior[enemywarrior[i]->opponent]->HP =  warrior[enemywarrior[i]->opponent]->HP - hpchange;
                        warrior[enemywarrior[i]->opponent]->judgedead();

                        fightanimation(enemywarrior[i], hpchange);//战斗动画

                        delayedneedle = 0;
                        delayedtime->start(1000);
                        loop.exec();
                        fightingm_pSoundEffect->stop();
                        fightbuttonhide(i);
                        if(enemywarrior[i]->judgefalse())//判断我方是否失败
                        {
                            mpmap->Clear();//数据清理
                            enemyrolemap->Clear();
                            m_pSoundEffect->stop();
                            ui->stackedWidget->setCurrentIndex(4);
                            QString file = ":/else/allwav/false.wav";
                            backgroundonplay(file);
                            return ;
                        }
                    }
                }
           }
        }
    }
    huihe = 1;//敌方回合结束，转换我方回合
    ui->stackedWidget_2->setCurrentIndex(1);
    labelinformation->show();
    labelinformation->setPixmap(QPixmap(":/whilefighting/allpicture/myhuihe.png"));//我方回合动画
    needle = 0;
    time->start(1000);
    this->remyhuihe();//转入我方回合的特殊地形对敌人的血量变化结算
}

void MainWindow::remyhuihe()//我方回合的特殊地形对敌人的血量变化结算+我方移动以及攻击次数、待机状态数据的更新
{
    for(int i=0;i<enemypeople;i++)
    {
        if(!enemywarrior[i]->isdead)
        {
            switch(mpmap->imagemap[enemywarrior[i]->x][enemywarrior[i]->y])
            {
            case 6:enemywarrior[i]->HP = enemywarrior[i]->HP +50;break;
            case 3:enemywarrior[i]->HP = enemywarrior[i]->HP - 30;break;
            case 10:enemywarrior[i]->HP = enemywarrior[i]->HP - 40;break;
            case 13:enemywarrior[i]->HP = enemywarrior[i]->HP - 50;break;
            case 16:enemywarrior[i]->HP = enemywarrior[i]->HP - 60;break;
            }
            enemywarrior[i]->judgedead();
        }
    }//可能存在的敌人血量数据更新
    for(int i=0;i<mypeople;i++)
    {
        if(!warrior[i]->isdead)
        {
            warrior[i]->movebefore=1;
            warrior[i]->fightbefore=1;
            warrior[i]->movecishu = 1;
            warrior[i]->fightcishu = 1;
            warrior[i]->isStandby = 0;
        }
    }//刷新次数
}

//该函数的思路是：（移动范围以四个方向的直线展示，从该战士为中心向四面展开，遇到阻碍地形、我方战士、敌方战士均会被阻挡
//1、判断移动范围的边界问题
//2、判断当前状态，如果为空，转2.1，如果为移动 则转2.2
//2.1 将当前状态改变为移动，移动次数减少1，移动范围显示黄色，在movefightmap上标记移动范围
//2.2 将当前状态改变为空，如果是移动前点击，则表示取消移动状态，移动次数恢复1，标记取消，如果是移动后调用，则移动次数不回复，地面标记仍消失
void MainWindow::movebutton()//移动按钮的槽函数
{
    int range = warrior[mysubmap[hang][lie]]->Movingrange;//移动范围的边界判断
    int minup,mindown,minleft,minright;
    minup = ((hang + range) < 15) ?  (hang + range): 14;
    mindown = ((hang - range) < 0) ?  0 : (hang - range);
    minleft = ((lie - range) < 0 ) ? 0 : (lie - range) ;
    minright = ((lie +range ) < 22) ? (lie+range): 21;
    if( state == "move" )//判断移动状态
    {
        if(warrior[mysubmap[hang][lie]]->movebefore)//判断是否实现了移动动画
        {
            warrior[mysubmap[hang][lie]]->movecishu++;
        }
        state.clear();//状态改变
        if(hang < minup)//数据以及地面标记消失
        {
            for(int i= hang+1 ; i<=minup;i++ )
            {
                if(mpmap->imagemap[i][lie] == 1 || mpmap->imagemap[i][lie] == 2 ||mpmap->imagemap[i][lie]==4 ||mpmap->imagemap[i][lie]==9 ||mpmap->imagemap[i][lie]==12 ||mpmap->imagemap[i][lie]==15|| enemysubmap[i][lie]>=0||mysubmap[i][lie]>=0)
                    break;
                button[i][lie]->setFlat(true);
                button[i][lie]->setStyleSheet("background:transparent;");
                movefightmap[i][lie] = -1;
            }
        }
        if(hang>mindown)
        {
            for(int i= hang-1;i >=mindown ;i--)
            {
                if(mpmap->imagemap[i][lie] == 1 || mpmap->imagemap[i][lie] ==2 ||mpmap->imagemap[i][lie]==4||mpmap->imagemap[i][lie]==9 ||mpmap->imagemap[i][lie]==12 ||mpmap->imagemap[i][lie]==15|| enemysubmap[i][lie]>=0||mysubmap[i][lie]>=0)
                    break;
                button[i][lie]->setFlat(true);
                button[i][lie]->setStyleSheet("background:transparent;");
                movefightmap[i][lie] = -1;
            }
        }
        if(lie<minright)
        {
            for(int i= lie+1 ;i<= minright ;i++)
            {
                if(mpmap->imagemap[hang][i] == 1 || mpmap->imagemap[hang][i] ==2 ||mpmap->imagemap[hang][i]==4||mpmap->imagemap[hang][i]==9 ||mpmap->imagemap[hang][i]==12 ||mpmap->imagemap[hang][i]==15|| enemysubmap[hang][i]>=0||mysubmap[hang][i]>=0)
                    break;
                button[hang][i]->setFlat(true);
                button[hang][i]->setStyleSheet("background:transparent;");
                movefightmap[hang][i] = -1;
            }
        }
        if(lie>minleft)
        {
            for(int i= lie-1 ;i>= minleft ;i--)
            {
                if(mpmap->imagemap[hang][i] == 1 || mpmap->imagemap[hang][i] ==2 ||mpmap->imagemap[hang][i]==4||mpmap->imagemap[hang][i]==9 ||mpmap->imagemap[hang][i]==12 ||mpmap->imagemap[hang][i]==15|| enemysubmap[hang][i]>=0||mysubmap[hang][i]>=0)
                    break;
                button[hang][i]->setFlat(true);
                button[hang][i]->setStyleSheet("background:transparent;");
                movefightmap[hang][i] = -1;
            }
        }
    }
    else//当前状态为空
    {
        if(warrior[mysubmap[hang][lie]]->movecishu != 0)//判断是否有移动次数
        {
            warrior[mysubmap[hang][lie]]->movecishu--;
            state = "move";
            if(hang < minup)//移动范围标记
            {
                for(int i= hang+1 ; i<=minup;i++ )
                {
                    if(mpmap->imagemap[i][lie] == 1 || mpmap->imagemap[i][lie] == 2 ||mpmap->imagemap[i][lie]==4 ||mpmap->imagemap[i][lie]==9 ||mpmap->imagemap[i][lie]==12 ||mpmap->imagemap[i][lie]==15|| enemysubmap[i][lie]>=0||mysubmap[i][lie]>=0)
                        break;
                    button[i][lie]->setFlat(false);
                    button[i][lie]->setStyleSheet("background-color:rgba(255,255,0,0.5)");
                    movefightmap[i][lie] = 1;
                }
            }
            if(hang>mindown)
            {
                for(int i= hang-1;i >=mindown ;i--)
                {
                    if(mpmap->imagemap[i][lie] == 1 || mpmap->imagemap[i][lie] ==2 ||mpmap->imagemap[i][lie]==4||mpmap->imagemap[i][lie]==9 ||mpmap->imagemap[i][lie]==12 ||mpmap->imagemap[i][lie]==15|| enemysubmap[i][lie]>=0||mysubmap[i][lie]>=0)
                        break;
                    button[i][lie]->setFlat(false);
                    button[i][lie]->setStyleSheet("background-color:rgba(255,255,0,0.5)");
                    movefightmap[i][lie] = 1;
                }
            }
            if(lie<minright)
            {
                for(int i= lie+1 ;i<= minright ;i++)
                {
                    if(mpmap->imagemap[hang][i] == 1 || mpmap->imagemap[hang][i] ==2 ||mpmap->imagemap[hang][i]==4||mpmap->imagemap[hang][i]==9 ||mpmap->imagemap[hang][i]==12 ||mpmap->imagemap[hang][i]==15|| enemysubmap[hang][i]>=0||mysubmap[hang][i]>=0)
                        break;
                    button[hang][i]->setFlat(false);
                    button[hang][i]->setStyleSheet("background-color:rgba(255,255,0,0.5)");
                    movefightmap[hang][i] = 1;
                }
            }
            if(lie>minleft)
            {
                for(int i= lie-1 ;i>= minleft ;i--)
                {
                    if(mpmap->imagemap[hang][i] == 1 || mpmap->imagemap[hang][i] ==2 ||mpmap->imagemap[hang][i]==4||mpmap->imagemap[hang][i]==9 ||mpmap->imagemap[hang][i]==12 ||mpmap->imagemap[hang][i]==15|| enemysubmap[hang][i]>=0||mysubmap[hang][i]>=0)
                        break;
                    button[hang][i]->setFlat(false);
                    button[hang][i]->setStyleSheet("background-color:rgba(255,255,0,0.5)");
                    movefightmap[hang][i] = 1;
                }
            }
        }
    }
}

//该函数的思路是：（箭塔有加成）
//1、判断是否是战斗状态，如果不是，转1.1，是则转1.2
//1.1 判断战斗次数是否足够，不足则点了个寂寞，足够则设置状态为战斗状态，对攻击范围进行标记
//1.2 判断战斗发生了没有，发生前点则是取消战斗状态以及战斗范围标记，如果是发生后点则取消战斗范围标记
void MainWindow::fightbutton()
{
    if(mpmap->imagemap[hang][lie]==5 && state != "fight" )//判断所在位置是否为箭塔
    {
        warrior[mysubmap[hang][lie]]->aggressivity+=10;
        warrior[mysubmap[hang][lie]]->defensivepower+=10;
        warrior[mysubmap[hang][lie]]->fightrange+=1;
    }
    int range = warrior[mysubmap[hang][lie]]->fightrange;
    int minup,mindown,minleft,minright;
    minup = ((hang + range) < 15) ?  (hang + range): 14;//判断战斗范围的边界效应
    mindown = ((hang - range) < 0) ?  0 : (hang - range);
    minleft = ((lie - range) < 0 ) ? 0 : (lie - range);
    minright = ((lie +range ) < 22) ? (lie+range): 21;
    if( state == "fight" )//当前状态为战斗状态
    {
        if(mpmap->imagemap[hang][lie]==5)//解除箭塔加成
        {
            warrior[mysubmap[hang][lie]]->aggressivity-=10;
            warrior[mysubmap[hang][lie]]->defensivepower-=10;
            warrior[mysubmap[hang][lie]]->fightrange-=1;
        }
        if(warrior[mysubmap[hang][lie]]->fightbefore)
        {
            warrior[mysubmap[hang][lie]]->fightcishu++;
        }
        state.clear();
        for(int i= mindown ; i<=minup;i++ )
        {
            for(int j = minleft ; j<= minright ; j++)
            {
                if(mpmap->imagemap[i][j] == 1 || mpmap->imagemap[i][j] ==2 ||mpmap->imagemap[i][j]==4||mpmap->imagemap[i][j]==9 ||mpmap->imagemap[i][j]==12 ||mpmap->imagemap[i][j]==15|| mysubmap[i][j]>=0)
                {

                }
                else{
                    button[i][j]->setFlat(true);
                    button[i][j]->setStyleSheet("background:transparent;");
                    movefightmap[i][j] = -1;
                }
            }
        }
    }
    else//当前状态为空
    {
        if(warrior[mysubmap[hang][lie]]->fightcishu != 0)//判断攻击次数
        {
            warrior[mysubmap[hang][lie]]->fightcishu--;
            state = "fight";
            for(int i= mindown ; i<=minup;i++ )//地面标记
            {
                for(int j = minleft ; j<= minright ; j++)
                {
                    if(mpmap->imagemap[i][j] == 1 || mpmap->imagemap[i][j] ==2 ||mpmap->imagemap[i][j]==4||mpmap->imagemap[i][j]==9 ||mpmap->imagemap[i][j]==12 ||mpmap->imagemap[i][j]==15|| mysubmap[i][j]>=0)
                    {

                    }
                    else{
                        button[i][j]->setFlat(false);
                        button[i][j]->setStyleSheet("background-color:rgba(255,0,0,0.5)");
                        movefightmap[i][j] = 1;
                    }
                }
            }
        }
    }
}

//待机按钮的设计思路是清空，将待机单位的攻击与移动的次数清空，修改待机状态+判断我方是否全部待机，是则轮到对方回合
void MainWindow::roleoverbutton()
{
    if(huihe == 1)//我方回合才有效果
    {
        int xuhao = mysubmap[hang][lie];//清空数据
        warrior[xuhao]->movecishu = 0 ;
        warrior[xuhao]->fightcishu = 0 ;
        warrior[xuhao]->isStandby = 1 ;
        button[hang][lie]->setFlat(false);
        button[hang][lie]->setStyleSheet("background-color:rgba(0,0,255,0.5)");
        bool test =1;
        for(int i = 0 ; i<mypeople ; i++)//判断我方战士是否全部待机
        {
            if(warrior[i]->isStandby ==0 )
            {
                test = 0 ;
            }
        }
        if(test)//如果全部待机则转到对对面回合，取消待机状态颜色标记
        {
            huihe = 2;
            for(int i = 0 ; i<mypeople ; i++)
            {
                button[warrior[i]->x][warrior[i]->y]->setFlat(true);
                button[warrior[i]->x][warrior[i]->y]->setStyleSheet("background:transparent;");
            }
            ui->stackedWidget_2->setCurrentIndex(1);
            labelinformation->show();
            labelinformation->setPixmap(QPixmap(":/whilefighting/allpicture/enemyhuihe.png"));
            needle=0;
            time->start(1000);
            delayedneedle = 0;
            delayedtime->start(1000);
            loop.exec();
            this->enemyhuihe();
        }
    }
}

//该函数的思路是:方便大作业的检查。由于游戏设计的机制是战胜了关卡才能升级，建议助教打过新手关卡（暗夜森林），这样兵种数量与种类限制放开一层，便于后续游戏性能的测试
//撤退就等于这关闯关失败，是不会有升级的。所以直接调用失败判断后的数据清空以及跳转到失败界面
void MainWindow::withdrawbutton()
{
    for(int i=0;i<enemypeople;i++)//人物隐藏
    {
        enemywarrior[i]->hide();
    }
    for(int i=0;i<mypeople;i++)//人物隐藏
    {
        warrior[i]->hide();
    }
    mpmap->Clear();//数据清空
    enemyrolemap->Clear();
    m_pSoundEffect->stop();
    ui->stackedWidget->setCurrentIndex(4);
    QString file = ":/else/allwav/false.wav";
    backgroundonplay(file);
    return ;
}
//绘画函数
//设计思路：与前面的事件过滤器结合，根据参数判断监视对象，在不同界面上绘画
void MainWindow::showPaint(QObject *watched)
{
    if(watched == ui->start)//开始界面的背景图
    {
        QPainter startwidget_painter(ui->start);
        QPainter name_painter(ui->start);
        startwidget_painter.drawPixmap(rect(),QPixmap(":/start/allpicture/startinterface.png"),QRect());
        name_painter.drawImage(QRect(0,50,1250,250),QImage(":/start/allpicture/gamename.png"));
        startwidget_painter.end();
        name_painter.end();
    }
    if(watched == ui->stageselect)//选关界面的背景图
    {
        QPainter selectwidget_painter(ui->stageselect);
        selectwidget_painter.drawPixmap(rect(),QPixmap(":/select/allpicture/selectinterface.png"),QRect());
        selectwidget_painter.end();
    }
    if(watched == ui->stackedWidget_2)//右侧信息栏的背景图
    {
        QPainter informationimage(ui->stackedWidget_2);
        informationimage.drawImage(QRect(0,0,150,750),QImage(":/fightinformation/allpicture/informationbackground.png"));
        informationimage.end();
    }
    if(watched == ui->firstsection)//战斗地图
    {
        QPainter informationimage(ui->firstsection);
        QString imgurl;
        switch(section)//战斗背景图（地板）
        {
            case 1:imgurl = ":/terrain/allpicture/grass.png";break;
            case 2:imgurl = ":/terrain/allpicture/redgrass.png";break;
            case 3:imgurl = ":/terrain/allpicture/whitegrass.png";break;
            case 4:imgurl = ":/terrain/allpicture/blackgrass.png";break;
        }
        informationimage.drawPixmap(rect(),QPixmap(imgurl),QRect());
        for(int i=0;i<15;i++)//战斗地形图
        {
            for(int j=0;j<22;j++)
            {
                if(mpmap->imagemap[i][j] != 0 && mpmap->imagemap[i][j] != 8 && mpmap->imagemap[i][j] != 11 && mpmap->imagemap[i][j] != 14)
                {
                    imgurl = photomapping(i,j,imgurl);//文件信息与图片信息转换函数
                    informationimage.drawImage(QRect(j* 50, i * 50, 50, 50),QImage(imgurl));
                }
            }
        }
        informationimage.end();
    }
}

