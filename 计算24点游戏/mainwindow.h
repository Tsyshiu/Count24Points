#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_pushButton_clicked();
    void newTurn();         //新一局
    void restartTurn();     //重新开始上一局
    void countDown();       //计时60s

private:
    std::vector<int> fourNums;
    Ui::MainWindow *ui;
    QMessageBox *choiceBox;
    QTimer *timer;
    int timeRemain;
    constexpr static const int TimeLimits=60;
};

//生成24点
bool generate24(const std::vector<int> &);
//检查计算正确性
bool check(std::vector<int>,QString);

#endif
