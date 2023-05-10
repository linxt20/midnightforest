#include "mainwindow.h"
#include "ui_mainwindow.h"

extern int needle;//三秒定时器指针
extern int enemyroletimeneedle;//敌人出场定时器指针
extern int upgrade;//升级 级别等于1+upgrade
extern int mysubmap[15][22];//我方战士地图
extern int enemysubmap[15][22];//敌方战士地图
extern int section;//关卡序号
extern int hang,lie;//目前点击格子的行列
extern int huihe;//现在的回合情况 0表示放置回合 1表示我方回合 2表示敌方回合
extern int nowchose;//放置回合时现在选中的战士的种类号
extern int mypeople;//我方战士人数
extern int enemypeople;//敌方战士人数

void MainWindow::uibaseoperator()//部分ui界面信息设置
{
    this->setWindowFlags(Qt::WindowCloseButtonHint);//取消窗口最大化按钮
    this->setFixedSize(this->width(),this->height());//限制窗口，无法被拉伸
    this->resize(1250,750);//设置窗口大小
    //设置xx回合框的大小
    labelinformation = new QLabel(ui->firstsection);
    labelinformation -> setGeometry(200,250,700,250);
    //设置战斗动画背景框信息
    labelfightbackground = new QLabel(ui->firstsection);
    labelfightbackground -> setGeometry(400,200,300,350);
    labelfightbackground->setScaledContents(true);
    labelfightbackground->setPixmap(QPixmap(":/whilefighting/allpicture/g.png"));
    labelfightbackground->hide();
    //设置战斗动画框信息
    labelfightskill = new QLabel(ui->firstsection);
    labelfightskill -> setGeometry(430,260,240,190);
    labelfightskill->setScaledContents(true);
    //设置战斗血量变化框信息
    labelHPchangetext = new QLabel(ui->firstsection);
    labelHPchangetext->setGeometry(420,460,180,60);
    labelHPchangetext->setText("对方损失生命值为");
    labelHPchangetext->setFont(QFont("宋体",12,QFont::Bold));
    labelHPchangetext->hide();

    //设置战斗血量变化信息显示
    lcdHPchangenum = new QLCDNumber(ui->firstsection);
    lcdHPchangenum->setDigitCount(2);
    lcdHPchangenum->setGeometry(600,460,80,60);
    lcdHPchangenum->hide();

    //给四个界面安装事件过滤器
    ui->start->installEventFilter(this);
    ui->stageselect->installEventFilter(this);
    ui->firstsection->installEventFilter(this);
    ui->stackedWidget_2->installEventFilter(this);

    //地图窗口分割成22*15个格子。每个格子都是50*50像素
    for(int i=0;i<15;i++)
    {
        for(int j=0;j<22;j++)
        {
            QPushButton *mapbutton= new QPushButton(ui->firstsection);
            mapbutton->setGeometry(j* 50, i * 50, 50, 50);
            mapbutton->setFlat(true);//按钮透明处理
            mapbutton->setStyleSheet("background:transparent;");//按钮透明处理
            button[i][j] = mapbutton;
            connect(button[i][j],&QPushButton::clicked,this,[=](){//地图格子关联点击判断函数
                clickedjudge(i,j);

            });
        }
    }

}

void MainWindow::backgroundonplay(QString file)//背景音乐播放函数
{
    m_pSoundEffect = new QSoundEffect(this);
    m_pSoundEffect->setSource(QUrl::fromLocalFile(file));
    m_pSoundEffect->setLoopCount(QSoundEffect::Infinite);//循环播放
    m_pSoundEffect->setVolume(1.0f);
    m_pSoundEffect->play();
}

void MainWindow::drawhero(int pointx,int pointy ,character *person)//绘画战士函数
{
    person->setGeometry(pointy*50,pointx*50,50,50);//战士大小
    QPixmap pix(person->imageaddress);//设置战士的图片
    pix = pix.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    person->setPixmap(pix);
    person->show();
    person->lower();//战士的label隐藏到地图按钮群下，使每次点击都是点击在按钮上
}

void MainWindow::informationshow(character *person)//战士信息展示函数
{
    ui->soldiername->setText(person->name);//战士的姓名
    ui->soldiername_2->setText(person->name);
    ui->lcdlevel->display(person->level);//战士的等级
    ui->lcdlife->display(person->HP);//战士的生命值
    ui->lcdfight->display(person->aggressivity);//战士的攻击力
    ui->lcddefence->display(person->defensivepower);//战士的防御力
    ui->lcdmovelength->display(person->Movingrange);//战士的移动距离
    ui->lcdfightlength->display(person->fightrange);//战士的攻击距离
    hang = person->x;//标记点击地点的横坐标
    lie = person->y;//标记点击地点的纵坐标
}

void MainWindow::same_operator_in_place(int type,QLCDNumber *lcd)//放置回合 点击人物与器侧边的lcd联动 同时可以可以取消上一次点击操作
{
    int lcdvalue = lcd->intValue();
    if(nowchose == type)//当你点击过一次，但是没有放置到地图上，再次点击可以取消上一次点击，人物个数恢复（即+1
    {
        lcdvalue++;
        lcd->display(lcdvalue);
        nowchose = 0;//目前选中的类型取消
    }
    else{//当你第一次点击该战士，战士数量减少1
        if(lcdvalue!=0)
        {
            lcdvalue--;
            lcd->display(lcdvalue);
            nowchose = type;//目前选中的类型号
        }
    }
}

void MainWindow::same_operator_in_continue(QString file)//胜利或者失败后的继续按钮，将会跳转到选关界面（第四关获胜后会有剧情动画）
{
    m_pSoundEffect->stop();
    ui->stackedWidget->setCurrentIndex(1);
    backgroundonplay(file);//背景音乐播放
}

//选择关卡的通用设计，点击进入新的关卡，会有数据更新，包括人数放置数量的重置（与目前等级有关）
//读取该关卡的地图文件，播放背景音乐，开始回合框动画，以及敌人出现动画
void MainWindow::same_operator_in_select(QString filename,QString file)
{
    m_pSoundEffect->stop();
    mpmap =new readfile(ui->firstsection);
    ui->lcdNumberjinzhan->display((int)(0.75+0.25*upgrade));//放置人数数量更新（与等级有关）
    ui->lcdNumbersheshou->display((int)(0.75+0.25*upgrade));
    ui->lcdNumberqishi->display((int)(0.75+0.25*upgrade));
    ui->lcdNumberfashi->display((int)(0.875+0.125*upgrade));
    ui->lcdNumberdunzhan->display((int)(0.875+0.125*upgrade));
    ui->lcdNumberboss->display(1);//主人公只有一个
    huihe=0;//放置回合
    mypeople=0;
    enemypeople= 0 ;

    for(int i=0;i<15;i++)//人物地图重置
    {
        for(int j=0;j<22;j++)
        {
            mysubmap[i][j]=-1;
            enemysubmap[i][j]=-1;
        }
    }

    mpmap->initbyfile(filename);//文件读取
    ui->stackedWidget->setCurrentIndex(2);//跳转到战斗地图
    ui->stackedWidget_2->setCurrentIndex(0);//右侧信息窗格跳转到放置界面
    labelinformation->show();
    backgroundonplay(file);//背景音乐播放
    needle = 0;
    time->start(1000);
    enemyroletimeneedle=0;//敌人出现动画
    enemyroletime->start(1000);

}

void MainWindow::textmapping(int i,int j)//地图与右侧信息框链接，点击地图上的方格块，可以在右侧窗格得到该地形的相关信息
{
    switch(mpmap->imagemap[i][j])
    {
    case 1 :ui->stackedWidget_3->setCurrentIndex(1);break;//都是stackwidget页面跳转
    case 2 :ui->stackedWidget_3->setCurrentIndex(2);break;
    case 3 :ui->stackedWidget_3->setCurrentIndex(3);break;
    case 4 :ui->stackedWidget_3->setCurrentIndex(4);break;
    case 5 :ui->stackedWidget_3->setCurrentIndex(5);break;
    case 6 :ui->stackedWidget_3->setCurrentIndex(6);break;
    case 7 :ui->stackedWidget_3->setCurrentIndex(7);break;
    case 0 :ui->stackedWidget_3->setCurrentIndex(0);break;
    case 9 :ui->stackedWidget_3->setCurrentIndex(9);break;
    case 10:ui->stackedWidget_3->setCurrentIndex(10);break;
    case 8 :ui->stackedWidget_3->setCurrentIndex(8);break;
    case 12:ui->stackedWidget_3->setCurrentIndex(12);break;
    case 13:ui->stackedWidget_3->setCurrentIndex(13);break;
    case 11:ui->stackedWidget_3->setCurrentIndex(1);break;
    case 15:ui->stackedWidget_3->setCurrentIndex(15);break;
    case 16:ui->stackedWidget_3->setCurrentIndex(16);break;
    case 14 :ui->stackedWidget_3->setCurrentIndex(14);break;
    }
}
//地图文件与图片信息的链接，根据文件上的信息找到相应的图片，转化成地图
QString MainWindow::photomapping(int i,int j,QString imgurl)
{
    switch( mpmap->imagemap[i][j])
    {
        case stone:imgurl = ":/terrain/allpicture/stone.png";break;
        case trap:imgurl = ":/terrain/allpicture/grasstrap.png";break;
        case bartizan:imgurl = ":/terrain/allpicture/bartizan.png";break;
        case granary:imgurl = ":/terrain/allpicture/granary.png";break;
        case river:imgurl = ":/terrain/allpicture/river.png";break;
        case wall:imgurl = ":/terrain/allpicture/wall.png";break;
        case bridge:imgurl = ":/terrain/allpicture/bridge.png";break;
        case redstone:imgurl = ":/terrain/allpicture/redstone.png";break;
        case redtrap:imgurl = ":/terrain/allpicture/redtrap.png";break;
        case whitestone:imgurl = ":/terrain/allpicture/whitestone.png";break;
        case whitetrap:imgurl = ":/terrain/allpicture/whitetrap.png";break;
        case blackstone:imgurl = ":/terrain/allpicture/blackstone.png";break;
        case blacktrap:imgurl = ":/terrain/allpicture/blacktrap.png";break;
    }
    return imgurl;//返回当前地点对应的图片路径
}
