#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <QMainWindow>
#include <qmqttclient.h>
#include <random>

constexpr quint8 QoS2 = 2;

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void setClientPort(int p);

private slots:
    void on_buttonConnect_clicked();

    void on_buttonQuit_clicked();

    void updateLogStateChange();

    void brokerDisconnected();

    void on_buttonPublish_clicked();

    void on_buttonPing_clicked();

    void errorHandler(const QMqttClient::ClientError &err);

    void subscribeFTPPort();

    void messageHandler(const QByteArray &message, const QMqttTopicName &topic);

private:
    Ui::MainWindow *ui;
    QMqttClient *m_client;
    QString current_dir;
    QLatin1String topic_command_port, topic_dataTransfer_port;
    QStringList files_to_get;
    std::random_device rd;

    enum ParseState { WAITING, SUCCESS, FAILURE };
    // 以下四个函数返回值：0表示等待远程返回结果，1表示处理正常结束，2表示处理错误
    ParseState parseCommand(const QString &commandStr);
    ParseState sendFiles(const QStringList &file_list);
    //这两个功能由broker处理
    inline ParseState getFiles(const QString &commandStr);
    inline ParseState listRemoteFiles(const QString &commandStr);

    using msgNoInt = quint16;
    msgNoInt msgNo_published, &msgNo_beg_published = msgNo_published;

    inline void QMessageBoxWrapper(const QString &&warning_message);

    inline void writeLog(const QString &&content);

    //static
    static const QString ClientState2String[];

};

#endif // MAINWINDOW_H
