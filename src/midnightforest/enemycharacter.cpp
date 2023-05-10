#include "enemycharacter.h"

extern int mysubmap[15][22];//我方战士地图
extern int enemysubmap[15][22];//地方战士地图
int Rownum=15,Colnum=22;//行列数
int Beginrow,Begincol,Endrow,Endcol;//终点坐标
int ddlstate=0;//判断路径是否存在
extern int upgrade;//升级 级别等于1+upgrade
extern int searchpath[15][22];//搜索地图 阻碍地形以及敌方单位都是该敌方单位的障碍

enemycharacter::enemycharacter(int _x,int _y,int _category,QWidget *parent):character(parent)//构造函数
{//录入敌方数据
    x=_x;//敌方的目前位置
    y=_y;
    switch(_category)//根据类型给不同的敌人录入数据
    {
    case 1:
        name = "privates";//姓名
        level = 1+upgrade;//等级
        HP = 80+20*upgrade;//生命值
        aggressivity=50+5*upgrade;//攻击力
        defensivepower=15+5*upgrade;//防御力
        Movingrange = 2;//移动距离
        fightrange = 1;//攻击距离
        imageaddress = ":/enemyrole/allpicture/Enemymelee.png";//战士图片地址
        gifaddress = ":/emeny/allgif/Enemymelee.gif";//战士攻击动画地址
        break;
    case 2:
        name = "Archer";//姓名
        level = 1+upgrade;//等级
        HP = 80+20*upgrade;//生命值
        aggressivity=45+5*upgrade;//攻击力
        defensivepower=15+5*upgrade;//防御力
        Movingrange = 2;//移动距离
        fightrange = 2;//攻击距离
        imageaddress = ":/enemyrole/allpicture/EnemyArcher.png";//战士图片地址
        gifaddress = ":/emeny/allgif/EnemyArcher.gif";//战士攻击动画地址
        break;
    case 3:
        name = "Archer";//姓名
        level = 1+upgrade;//等级
        HP = 100+20*upgrade;//生命值
        aggressivity=60+5*upgrade;//攻击力
        defensivepower=20+5*upgrade;//防御力
        Movingrange = 3;//移动距离
        fightrange = 1;//攻击距离
        imageaddress = ":/enemyrole/allpicture/EnemyKnight.png";//战士图片地址
        gifaddress = ":/emeny/allgif/EnemyKnight.gif";//战士攻击动画地址
        break;
    case 4:
        name = "Warlock";//姓名
        level = 1+upgrade;//等级
        HP = 90+20*upgrade;//生命值
        aggressivity=70+5*upgrade;//攻击力
        defensivepower=10+5*upgrade;//防御力
        Movingrange = 2;//移动距离
        fightrange = 2;//攻击距离
        imageaddress = ":/enemyrole/allpicture/Enemymage.png";//战士图片地址
        gifaddress = ":/emeny/allgif/Enemymage.gif";//战士攻击动画地址
        break;
    case 5:
        name = "Walldefender";//姓名
        level = 1+upgrade;//等级
        HP = 150+30*upgrade;//生命值
        aggressivity=45+5*upgrade;//攻击力
        defensivepower=30+5*upgrade;//防御力
        Movingrange = 2;//移动距离
        fightrange = 1;//攻击距离
        imageaddress = ":/enemyrole/allpicture/Enemyshield.png";//战士图片地址
        gifaddress = ":/emeny/allgif/Enemyshield.gif";//战士攻击动画地址
        break;
    case 6:
        name = "Zhuge Liang";//姓名
        level = 1+upgrade;//等级
        HP = 300+40*upgrade;//生命值
        aggressivity=70+5*upgrade;//攻击力
        defensivepower=30+5*upgrade;//防御力
        Movingrange = 2;//移动距离
        fightrange = 1;//攻击距离
        imageaddress = ":/enemyrole/allpicture/enemyboss.png";//战士图片地址
        gifaddress = ":/emeny/allgif/enemybossgif.gif";//战士攻击动画地址
        break;
    }
}
bool enemycharacter::judgefalse()//判断我方失败函数，当我方全部死亡时，则我方失败
{
    for(int i=0;i<15;i++)
    {
        for(int j=0;j<22;j++)
        {
            if(mysubmap[i][j]>=0)//存在我方战士 返回false
               return false;
        }
    }
    return true;
}

bool enemycharacter::canplace(int prePosValue,int row,int col)//判断当前点能否走通
{
    if(row>=0 &&col>=0 &&row<Rownum &&col<Colnum &&searchpath[row][col]!=-1)//未越界且不是墙(-1)
    {
        if(searchpath[row][col]==0) return true;
        else
        {
            return (prePosValue + 1)<searchpath[row][col];// 更近的路径,也算可走通
        }
    }
    return 0;
}


void enemycharacter::search(stack<Postion> &path,int row,int col)//深搜递归
{
    if(row==Endrow &&col==Endcol)//是目标终点
    {
        if(path.size()<MinPATH.size() || MinPATH.empty())//更短的路径
        {
            ddlstate=1;
            MinPATH = path;
        }
        return;
    }

    int r,c,num;
    num=searchpath[row][col];
    r=row-1; c=col; //上
    {
        if(canplace(num,r,c))
        {
            searchpath[r][c]=num+1;
            path.push(Postion(r,c));//[r,c]进栈
            search(path,r,c); //深度优先搜索
            path.pop();//[r,c]出栈,退回到栈顶为[row,col]
        }
    }
    r=row; c=col+1;//右
    {
        if(canplace(num,r,c))
        {
            searchpath[r][c]=num+1;
            path.push(Postion(r,c));//进栈
            search(path,r,c); //深度优先搜索
            path.pop();//出栈
        }
    }
    r=row+1;  c=col;//下
    {
        if(canplace(num,r,c))
        {
            searchpath[r][c]=num+1;
            path.push(Postion(r,c));//进栈
            search(path,r,c); //深度优先搜索
            path.pop();//出栈
        }
    }
    r=row;  c=col-1;//左
    {
        if(canplace(num,r,c))
        {
            searchpath[r][c]=num+1;
            path.push(Postion(r,c));//进栈
            search(path,r,c); //深度优先搜索
            path.pop();//出栈
        }
    }
}


//判断攻击范围内是否有我方战士，有则记录我方战士序号，判断方式是从内圈开始一圈圈搜索，只有1圈和两圈两种情况
bool enemycharacter::judgefightrange()
{
    for(int i = x - 1; i<= x +1 ; i++)
    {
        for(int j= y-1;j<= y+1 ; j++)
        {
            if(mysubmap[i][j]>=0)
            {
                opponent = mysubmap[i][j];//记录我方战士序号
                return true;
            }
        }
    }
    if(fightrange == 2)
    {
        for(int i = x-2; i<= x+2 ; i++)
        {
            if(mysubmap[i][y+2]>=0)
            {
                opponent = mysubmap[i][y+2];
                return true;
            }
            if(mysubmap[i][y-2]>=0)
            {
                opponent = mysubmap[i][y-2];
                return true;
            }
        }
        for(int i= y-1;i<=y+1;i++)
        {
            if(mysubmap[x+2][i]>=0)
            {
                opponent = mysubmap[x+2][i];
                return true;
            }
            if(mysubmap[x-2][i]>=0)
            {
                opponent = mysubmap[x-2][i];
                return true;
            }
        }
    }
    return false;
}

void enemycharacter::find(int pointx, int pointy)//寻找抵达我方战士的最短路径
{
    MinPATH = stack<Postion>();//记录最短路线
    ddlstate = 0 ;//判断是否存在路线
    Beginrow = x;//敌方战士的位置是起始点
    Begincol = y;
    Endrow = pointx;//我方战士的位置是终点
    Endcol = pointy;
    searchpath[Beginrow][Begincol]=1;//起始点开始记录
    stack<Postion> path = stack<Postion>();//记录过程路径
    search(path,Beginrow,Begincol);
    //一个迷宫搜索路径后的最终状态，起始点接通与否
    int step_num=searchpath[Endrow][Endcol]-1;
    if(ddlstate==1)
    {
        if(step_num < minstep)//minpath只记录最短的路径
        {
            minstep = step_num;
            printPath(MinPATH);//拷贝路径
        }
    }
}

void enemycharacter::printPath(stack<Postion> MinPATH)
{
    while (!MinPATH.empty())//由于移动范围为1-3，只需记录前三个点，若不到三个点就全部记录
    {
        switch (searchpath[MinPATH.top().X][MinPATH.top().Y]-1) {
        case 3 :
            pathx[2]= MinPATH.top().X;
            pathy[2]= MinPATH.top().Y;
            break;
        case 2 :
            pathx[1]= MinPATH.top().X;
            pathy[1]= MinPATH.top().Y;
            break;
        case 1:
            pathx[0]= MinPATH.top().X;
            pathy[0]= MinPATH.top().Y;
            break;
        }
        MinPATH.pop();
    }
}
//敌方ai智能选择走法，由于双方都是直线行走，行走步数主要参考因素为：
//眼前的路径直线长度、该战士的移动范围、以及该战士的攻击范围（如果攻击范围为两格，则会走到离我方战士有两格的距离，一格则距离一格）
int enemycharacter::movepoint()
{
    if((x == pathx[1] || y== pathy[1] )&& minstep-fightrange>=2 )//x与y的判断是直线参考，movingrange是移动范围参考，minstep-fightrange则是攻击距离的参考
    {
        if(Movingrange == 3 && (x == pathx[2] || y== pathy[2]) && minstep-fightrange>=3)
        {
            return  2;
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

void enemycharacter::judgedead()//判断该战士是否死亡，以及死亡后的对应操作
{
    if(HP<=0)
    {
        HP = 0;
        hide();
        enemysubmap[x][y]=-1;
        isdead = 1;
    }
}

