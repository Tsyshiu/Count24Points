

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
double number[CONT];
std::vector<QString> expression;
//QString expression[CONT];
bool m_judge = false; //判断是否有解。
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
    for(int i=0; i < n && m_judge==false; i++)//查找
    {
        for (int j = i + 1; j < n && m_judge==false; j++)//与其后面的查找进行计算
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

bool generate24(const std::vector<int> &num4)
{
    count=0;
    expression.clear();
    expression.resize(CONT);
    m_judge=false;

    std::copy(num4.begin(),num4.end(),number);

    for (int i = 0; i < CONT; i++)
    {
        expression[i].setNum(num4[i],10);
    }

    qDebug()<<endl;
    Find(CONT) ;
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
