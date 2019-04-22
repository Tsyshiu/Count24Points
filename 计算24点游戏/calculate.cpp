#include <QString>
#include <vector>
#include <iostream>
#include <cmath>
#include <cfloat>
#include <QDebug>

#include "mainwindow.h"

constexpr double epsilon = DBL_EPSILON*8;
constexpr int CONT = 4;
constexpr int VOLUE = 24;
static std::vector<QString> expression(CONT);
static double number[CONT];
static bool m_judge = false; //判断是否有解。
int count;

void Find(int n)
{
    if (n == 1)
    {
        if ( fabs(number[0] - VOLUE) <= epsilon )
        {
            qDebug() << expression[0] << "\t\t";
            m_judge = true;
            count++;
            if((count % 3)==0) //使输出时每行三个表达式
                qDebug()<<endl;
        }
        else
        { }
    }
    for(int i=0; i < n /*&& m_judge==false*/; i++)//查找
    {
        for (int j = i + 1; j < n /*&& m_judge==false*/; j++)//与其后面的查找进行计算
        {
            double a, b;
            QString expressiona, expressionb;
            a = number[i];
            b = number[j];
            number[j] = number[n - 1];
            expressiona = expression[i];
            expressionb = expression[j];
            expression[j] = expression[n - 1];
            expression[i]= '('+ expressiona + '+' + expressionb + ')';
            number[i] = a + b;
            Find(n-1);
            expression[i]='('+ expressiona+ '-' + expressionb + ')';
            number[i] = a - b;
            Find(n-1);
            expression[i] = '('+expressionb + '-' + expressiona + ')';
            number[i] = b -a;
            Find(n-1);
            expression[i]= '('+ expressiona +'*'+ expressionb+ ')';
            number[i]=a*b;
            Find(n-1);
            if (b != 0)
            {
                expression[i] ='('+expressiona+'/' + expressionb + ')';
                number[i] = a / b;
                Find(n-1);
            }
            if (a != 0)
            {
                expression[i]='('+expressionb + '/'+ expressiona + ')';
                number[i] = b / a;
                Find(n-1);
            }
            number[i] =a;
            number[j]=b;
            expression[i] = expressiona;
            expression[j] = expressionb;
        }
    }
}

//生成24点
bool generate24(const std::vector<int> &fourNums)
{
    count=0;
    expression.clear();
    m_judge=false;  //初始化

    std::copy(fourNums.begin(),fourNums.end(),number);  //int转到double

    for (int i = 0; i < CONT; i++)
    {
        expression[i].setNum(fourNums[i],10);   //int转string
    }

    qDebug()<<endl;
    Find(CONT);//调用find，find能输出所有的24点算式
//    if(m_judge==true)
//    {
//        qDebug() << "\n成功！" << endl;
//        qDebug()<<"总共的计算方法共有： "<<count<<endl;
//    }
//    else
//    {
//        qDebug() << "失败！" << endl;
//    }
    return m_judge;
}
