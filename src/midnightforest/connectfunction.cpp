#include "mainwindow.h"
#include "ui_mainwindow.h"

extern int needle;//3秒定时器的指针
extern int enemyroletimeneedle;//敌人出场指针
extern int delayedneedle;//敌人动画线程堵塞处指针
extern int upgrade;//升级 级别等于1+upgrade
extern int enemysubmap[15][22];//敌方战士地图
extern int section;//关卡序号
extern int hang,lie;//目前点击格子的行列
extern int huihe;//现在的回合情况 0表示放置回合 1表示我方回合 2表示敌方回合
extern int enemypeople;//敌方战士人数

//开始游戏按钮 点击后会跳转到剧情动画界面大约30秒的剧情动画采用gif动画与wav文件同时播放的来代替MP4，减少了不必要的解码器
void MainWindow::on_startgame_clicked()
{
    m_pSoundEffect->stop();
    ui->stackedWidget->setCurrentIndex(5);//跳转到剧情界面
    potmovie = new QMovie(":/pot/allgif/longlongago.gif");//gif动画
    potmovie->setSpeed(110);
    ui->label_6->setScaledContents(true);
    ui->label_6->setMovie(potmovie);
    potmovie->start();
    QString file = ":/pot/allwav/longlong.wav";//wav音频
    backgroundonplay(file);
}

void MainWindow::timeoperator()//3秒定时器槽函数 主要作用是定时将显示的动画窗口隐藏
{
    needle++;
    if(needle == 3)
    {
        labelinformation->hide();
        labelfightbackground->hide();
        labelfightskill->hide();
        labelHPchangetext->hide();
        lcdHPchangenum->hide();
        time->stop();
    }
}

//敌人显示动画 敌人会在放置回合动画结束的瞬间读取敌人信息文件，通过信息转换显示并绘制出来
void MainWindow::enemyroletimeoperator()
{
    enemyroletimeneedle++;
    if(enemyroletimeneedle == 3)
    {

        enemyrolemap = new readfile(ui->firstsection);
        QString filename;
        switch (section)//根据关卡的不同会有不同的敌人放置，为了让助教更方便的观看各个关卡的性能，本游戏没有限制闯关顺序
        {               //推荐的闯关顺序为  暗夜森林（新手） 暗夜森林（正式） 烽火赤壁 凋零冰川 逆流之光
        case 1:
            if(upgrade == 0)//新手关卡，打过后人物数量和种类会增加，不建议一上来就打后面的大关
            {
                filename= ":/map/allmaptxt/firstsectionrole.txt";//仅有1-2位敌方单位
            }
            else
            {
                filename = ":/map/allmaptxt/firstsection1.txt" ;//10位敌方单位
            }
            break;
        case 2:filename = ":/map/allmaptxt/secondsectionrole.txt";break;
        case 3:filename =":/map/allmaptxt/thirdsectionrole.txt";break;
        case 4:filename = ":/map/allmaptxt/fourthsectionrole.txt";break;
        }
        enemyrolemap->initbyfile(filename);//读取敌人战士文件

        for(int j = 0;j<22;j++)
        {
            for(int i=0;i<15;i++)
            {
                if(enemyrolemap->imagemap[i][j]>0)//根据敌人地图文件 扫描一遍即可画好敌人单位
                {
                    enemywarrior[enemypeople] = new enemycharacter(i,j,enemyrolemap->imagemap[i][j],ui->firstsection);
                    enemysubmap[i][j] = enemypeople;
                    drawhero(i,j,enemywarrior[enemypeople]);
                    enemypeople++;//记录敌人战士数量
                }
            }
        }
        enemyroletime->stop();
    }
}

void MainWindow::delayedtimeoperator()//延迟定时器槽函数
{
    delayedneedle++;
    if(delayedneedle == 4)//阻断线程四次超时信号
    {
        loop.quit();
        delayedtime->stop();
    }
}
//第一关的选关按钮 设置关卡数section 开始播放放置回合动画 以及读取地图地形文件，播放背景音乐
void MainWindow::on_midnightforest_clicked()
{
    section =1;
    QString filename(":/map/allmaptxt/firstsection.txt");
    labelinformation->setPixmap(QPixmap(":/whilefighting/allpicture/placehuihe.png"));
    QString file = ":/section/allwav/first.wav";
    same_operator_in_select(filename,file);//读取地图地形文件，播放背景音乐的统一操作函数
}
//第二关的选关按钮 设置关卡数section 开始播放放置回合动画 以及读取地图地形文件，播放背景音乐
void MainWindow::on_BeaconRedCliff_clicked()
{
    section =2;
    QString filename(":/map/allmaptxt/secondsection.txt");
    labelinformation->setPixmap(QPixmap(":/whilefighting/allpicture/placehuihe.png"));
    QString file = ":/section/allwav/second.wav";
    same_operator_in_select(filename,file);
}
//第三关的选关按钮 设置关卡数section 开始播放放置回合动画 以及读取地图地形文件，播放背景音乐
void MainWindow::on_witheringglacier_clicked()
{
    section =3;
    QString filename(":/map/allmaptxt/thirdsection.txt");
    labelinformation->setPixmap(QPixmap(":/whilefighting/allpicture/placehuihe.png"));
    QString file = ":/section/allwav/third.wav";
    same_operator_in_select(filename,file);
}
//第四关的选关按钮 设置关卡数section 开始播放放置回合动画 以及读取地图地形文件，播放背景音乐
void MainWindow::on_countcurrentlight_clicked()
{
    section =4;
    QString filename(":/map/allmaptxt/fourthsection.txt");
    labelinformation->setPixmap(QPixmap(":/whilefighting/allpicture/placehuihe.png"));
    QString file = ":/section/allwav/fourth.wav";
    same_operator_in_select(filename,file);
}
//结束放置按钮 点击后右侧信息栏会跳转到关卡介绍 开始我方回合动画
void MainWindow::on_overplace_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(1);
    ui->stackedWidget_4->setCurrentIndex(section-1);
    labelinformation->show();
    labelinformation->setPixmap(QPixmap(":/whilefighting/allpicture/myhuihe.png"));
    needle = 0;
    huihe = 1;
    time->start(1000);
}
//剧情界面的跳过按钮 点击即可结束剧情动画，跳转到选关界面，并播放背景音乐
void MainWindow::on_jump_clicked()
{
    potmovie = nullptr;
    m_pSoundEffect->stop();
    ui->stackedWidget->setCurrentIndex(1);
    QString file = ":/else/allwav/select.wav";
    backgroundonplay(file);
}
