#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <random>
#include <QDebug>
#include <QDesktopWidget>

std::default_random_engine engine(time(nullptr));

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),choiceBox(new QMessageBox(this))
{
    ui->setupUi(this);
    setFixedSize(586,377);

    QDesktopWidget *deskWgt = QApplication::desktop();
    const int ScrWidth=deskWgt->width();
    const int ScrHeight=deskWgt->height();

    QPixmap pixmap(":/back.jpg");
    QPalette *palette=new QPalette(this->palette());
    palette->setBrush(QPalette::Background,QBrush(pixmap));
    setPalette(*palette);

    ui->pushButton->setAttribute(Qt::WA_TranslucentBackground);
    ui->label_1->setAttribute(Qt::WA_TranslucentBackground);
  ui->label_2->setAttribute(Qt::WA_TranslucentBackground);
  ui->label_3->setAttribute(Qt::WA_TranslucentBackground);
  ui->label_4->setAttribute(Qt::WA_TranslucentBackground);
    connect(this->choiceBox,&QMessageBox::buttonClicked,[=](QAbstractButton *button)
            {
                switch(choiceBox->standardButton(button))
                {
                case QMessageBox::Reset:
                    qDebug()<<"reset";
                    restartTurn();
                    break;
                case QMessageBox::Ok:
                    qDebug()<<"Ok";
                    newTurn();
                    break;
                default:
                    qApp->quit();
                }
            });

    choiceBox->setStandardButtons(QMessageBox::Reset|QMessageBox::Ok|QMessageBox::Close);
    choiceBox->setButtonText(QMessageBox::Reset,"重试");
    choiceBox->setButtonText(QMessageBox::Ok,"新一轮游戏");
    choiceBox->setButtonText(QMessageBox::Close,"退出");
    choiceBox->setDefaultButton(QMessageBox::Ok);
    choiceBox->setEscapeButton(QMessageBox::Close);

    QMessageBox startUp(QMessageBox::NoIcon,"24点", "", QMessageBox::Ok|QMessageBox::Close,this);
    startUp.setInformativeText("您是否要开始24点小游戏？(点击详情获取游戏帮助)");
    startUp.setDetailedText("您将在屏幕上方随机获得4个数字，根据24点规则"
                            "在输入框中输入您得出的计算式，并点击√完成，如：\n"
                            "4,6,2,2, → 您输入：4*6*2/2 即通过此次24点游戏\n"
                            "注意您只有60秒时间完成一轮24点!!\n"
                            "之后您可以选择新一轮24点，或者退出游戏。\n"
                            "输入支持：0,1,2...+-*/()（）×÷\n");

    startUp.setButtonText(QMessageBox::Ok,"开始游戏");
    startUp.setButtonText(QMessageBox::Close,"退出");
    startUp.setDefaultButton(QMessageBox::Ok);
    startUp.setEscapeButton(QMessageBox::Close);
    startUp.move(ScrWidth/2-40,ScrHeight/2-29);
    int ret=startUp.exec();
    if(ret==QMessageBox::Close )
    {
        QTimer::singleShot(0,qApp,QCoreApplication::quit);
    }
    else
    {
        timer=new QTimer(this);
        connect(timer,&QTimer::timeout,this,&countDown);
        show();
        newTurn();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newTurn()
{
    ui->lineEdit->clear();
    std::uniform_int_distribution<> dis(1, 13);

    do{
        num4.clear();
        for (int n=0; n<4; ++n)
            num4.push_back(dis(engine));
    }while(generate24(num4)!=true);

    ui->label_1->setText(QString().setNum(num4[0]));
    ui->label_2->setText(QString().setNum(num4[1]));
    ui->label_3->setText(QString().setNum(num4[2]));
    ui->label_4->setText(QString().setNum(num4[3]));
    timeRemain=TimeLimits;
    timer->start(1000);

}

void MainWindow::restartTurn()
{
    ui->lineEdit->clear();
    timeRemain=TimeLimits;
    timer->start(1000);
}

void MainWindow::countDown()
{
    timeRemain--;
    if(timeRemain==-1)
      {
        timer->stop();
        choiceBox->setText("超时！");
        choiceBox->exec();
    }
    else
    {
        ui->countdown->display(timeRemain);
    }

}

void MainWindow::on_pushButton_clicked()
{
    timer->stop();
    if(check(num4,ui->lineEdit->text()))
    {
        choiceBox->setText("24点！祝贺！");
    }
    else
    {
        choiceBox->setText("失败！");
    }
    choiceBox->exec();
}
