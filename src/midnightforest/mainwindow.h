#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QFile>
#include <QDebug>
#include <QString>
#include <QTimer>
#include <QLabel>
#include <QPropertyAnimation>
#include <QMovie>
#include <QEventLoop>
#include <QLCDNumber>
#include <QFont>
#include <QtMultimedia/QSoundEffect>

#include "readfile.h"//文件读取
#include "mycharacter.h"//我方战士类
#include "enemycharacter.h"//敌方战士类

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;//ui界面
    readfile *mpmap;//读取地图文件
    readfile *enemyrolemap;//读取敌人位置文件
    QSoundEffect* m_pSoundEffect;//背景音乐
    QSoundEffect* fightingm_pSoundEffect;//战斗音效

public:
    QPainter *painter;//绘图画板
    QTimer *time;//三秒定时器
    QTimer *enemyroletime;//敌人出现定时器
    QTimer *delayedtime;//程序线程堵塞计时器
    QLabel *labelinformation;//回合框
    QLabel *labelfightbackground;//战斗动画背景图
    QLabel *labelfightskill;//战斗动画播放框
    QLabel *labelHPchangetext;//战斗动画血量结算框
    QLCDNumber *lcdHPchangenum;//战斗动画血量结算显示器
    QEventLoop loop;//线程循环堵塞装置
    QMovie * movie;//战斗动画
    QMovie *potmovie;//剧情动画
    mycharacter * warrior[20];//我方战士label类型数组（我方总数不超过20）
    enemycharacter *enemywarrior[20];//敌方战士label类型数组（敌人总数不超过20）
    QPushButton *button[16][23];//战斗地图方格按钮二维数组

public:
    MainWindow(QWidget *parent = nullptr);//构造函数
    ~MainWindow();//析构函数

    void showPaint(QObject *watched);//绘画函数
    bool eventFilter(QObject *watched, QEvent *event);//监视器，判断当前事件
    void clickedjudge(int i,int j);//地图点击判断

    void enemyhuihe();//我方回合结算+敌人ai设计
    void remyhuihe();//敌方回合结算+我方数据更新

    void uibaseoperator();//部分界面ui设计函数

    void backgroundonplay(QString file);//音乐播放函数

    void drawhero(int pointx,int pointy , character *person);//绘制我方与敌方战士函数
    void informationshow(character *person);//信息栏数据显示函数

    void textmapping(int i,int j);//地图信息栏跳转函数
    QString photomapping(int i,int j,QString imgurl);//文件信息与地图图片信息映射函数

    void same_operator_in_select(QString filename,QString file);//选关界面，四个关卡按钮的通用设计
    void same_operator_in_place(int type,QLCDNumber *lcd);//放置回合，六个人物放置操作按钮的通用设计
    void same_operator_in_continue(QString file);//胜利和失败界面的继续按钮的通用设计

    void mymoveanimation(int i,int j);//我方战士移动动画
    void enemymoveanimation(int i,int num);//敌方战士移动动画
    void mymovefinish(int i,int j);//我方战士移动动画结束
    void enemymovefinish(int i,int num);//敌方战士移动动画结束

    void fightanimation(character *person,int hpchange);//战斗动画
    void fightbuttonshow(int i);//战斗地板显色
    void fightbuttonhide(int i);//战斗结束地板显色消失
    void findnearperson(int i);//敌方寻找最近的我方单位
    void fightingmusic(QString type);//战斗音效

public slots:
    void movebutton();//对应战斗窗口的移动按钮
    void fightbutton();//对应战斗窗口的战斗按钮
    void roleoverbutton();//对应战斗窗口的待机按钮
    void withdrawbutton();//对应战斗窗口的撤退按钮

public slots:
    void on_startgame_clicked();//“开始游戏”按钮自动关联

    void timeoperator();//3秒定时器槽函数
    void enemyroletimeoperator();//敌人出现定时器
    void delayedtimeoperator();//线程堵塞计时器

    void on_midnightforest_clicked();//第一关暗夜森林选关按钮
    void on_BeaconRedCliff_clicked();//第二关烽火赤壁选关按钮
    void on_witheringglacier_clicked();//第三关凋零冰川选关按钮
    void on_countcurrentlight_clicked();//第四关逆流之光选关按钮

    void on_overplace_clicked();//结束放置按钮
    void on_jump_clicked();//剧情界面跳过按钮

};
#endif // MAINWINDOW_H
