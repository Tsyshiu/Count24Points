#include "mainwindow.h"
#include <iostream>
#include <QString>
#include <string>
#include <QStack>
#include <QDebug>
#include <exception>
#include <cfloat>
constexpr double epsilon = DBL_EPSILON*8;

inline int oc(const char &x)            //返回运算符优先级
{
     switch (x)
     {
     case '+':
     case '-':return 1;
     case '*':
     case '/':return 2;
     case '(':return 3;
     case ')':return 4;
     case '=':return 5;
     }
}

inline void resout(QStack<double> &N, QStack<char> &M)
{
     double b = N.pop();
     double a = N.pop();
     switch (M.pop())
     {
     case '+':N.push(a + b); break;
     case '-':N.push(a - b); break;
     case '*':N.push(a * b); break;
     case '/':N.push(a / b); break;
     default:;
     }
}

double calculate(std::string expr,std::vector<int> &numsGot)
{
     QStack<double> N;//数字栈
     QStack<char> M;//存符号

     //检查输入合法性
     if((*expr.rbegin())!='=')
         throw std::logic_error("表达式不合法");

     if(expr.size()==1)
         throw std::logic_error("表达式不合法");

     for (unsigned int i=0; i < expr.size()-1; i++)
     {
          if(!( (expr[i]>='0'&&expr[i]<='9') ||expr[i]=='*'||expr[i]=='+'||expr[i]=='-'||expr[i]=='/' ||expr[i]=='('||expr[i]==')') )
          throw std::logic_error("表达式不合法");
     }

     //开始计算
     for (unsigned int i = 0; i < expr.size();i++ )
     {
          std::string num;
          if(isdigit(expr[i]))//判断是否为数字
          {
                do {
                     num += expr[i];
                     i++;
                } while (isdigit(expr[i]));
                N.push(atof(num.c_str()));//将数字存入数字栈
                numsGot.push_back(atoi(num.c_str()));
          }
          //若不是数字
          switch (oc(expr[i]))//判断是什么运算符
          {
          case 1://若为+ -
                 while (!M.isEmpty() && oc(M.top()) <= 2)
                 {
                      resout(N, M);//计算函数
                 }
                M.push(expr[i]);
                break;
          case 2://若为* /
                     while (!M.isEmpty() && oc(M.top()) == 2)
                     {
                          resout(N, M);
                     }
                M.push(expr[i]);
                break;
          case 3://若为（
                M.push('(');
                break;
          case 4://若为）
                while (!M.isEmpty() && oc(M.top()) != 3)
                  resout(N, M);
                M.pop();
                break;
          case 5://=
                while (!M.isEmpty())
                    resout(N, M);
          }
     }
    return N.pop();
}



bool check(std::vector<int> numsOrig,QString expr)
{
     //cout << "请输入要计算的表达式，不允许有空格，以‘=’结尾: " << endl;
     expr.append('=');
     expr.replace(QChar(L'（'),QChar('('));
     expr.replace(QChar(L'）'),QChar(')'));
     expr.replace(QChar(L'×'),QChar('*'));
     expr.replace(QChar(L'÷'),QChar('/'));
     try {
         std::vector<int> numsGot;
         auto result = calculate(expr.toStdString(),numsGot);
         if(result - 24 <epsilon)
         {
             std::sort(numsGot.begin(),numsGot.end());
             std::sort(numsOrig.begin(),numsOrig.end());
             if(numsGot==numsOrig)
                return true;
         }

     }
     catch (std::logic_error &e)
     {
          qDebug() << e.what();
     }
    return false;
}
