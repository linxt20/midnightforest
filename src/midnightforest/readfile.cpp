#include "readfile.h"
#include <QDebug>

readfile::readfile(QObject *parent) : QObject(parent)//构造函数，初始化变量
{
    mrow = 15;
    mcol = 22;
    imagemap = nullptr;
}

readfile::~readfile()//析构函数
{
    Clear();
}

void readfile::initbyfile(QString filename)//文件读取函数
{
    QFile file(filename);//打开文件
    if(!file.open(QIODevice::ReadOnly))//判断文件是否打开
    {
        return;
    }

    QByteArray arr = file.readAll();//存取文件中的信息
    arr.replace("\r\n","\n");
    QList<QByteArray> linelist = arr.split('\n');
    imagemap = new int*[mrow];
    for(int i=0;i<mrow;i++)
    {
        QList<QByteArray> colist = linelist[i].split(',');
        imagemap[i] = new int[mcol];

        for(int j=0;j<mcol;j++)
        {
            imagemap[i][j] = colist[j].toInt();//将文件中的信息对应位置存在二维数组中
        }
    }

    file.close();
}

void readfile::Clear()//删除清理数据
{
    if(imagemap != nullptr)
    {
        for(int i=0;i<mrow;i++)
        {
            delete [] imagemap[i];
        }
        delete [] imagemap;
    }
}
