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

signals:
    void startNewTurn();


private slots:
    void on_pushButton_clicked();
    void newTurn();
    void restartTurn();
    void countDown();

private:
    std::vector<int> num4;
    Ui::MainWindow *ui;
    QMessageBox *choiceBox;
    QTimer *timer;
    int timeRemain;
    constexpr static const int TimeLimits=60;
};

bool generate24(const std::vector<int> &num4);
bool check(std::vector<int>,QString expr);

#endif
