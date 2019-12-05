#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QtCore/QDateTime>
#include <QtMqtt/qmqttclient.h>
#include <QtMqtt/qmqttpublishproperties.h>
#include <QtWidgets/QMessageBox>
#include <cassert>

const QString MainWindow::ClientState2String[] = {QLatin1String("Disconnected"),
                                                  QLatin1String("Connecting"),
                                                  QLatin1String("Connected")
                                                 };

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), current_dir(getenv("USERPROFILE")),
      topic_command_port("command"), topic_dataTransfer_port("dataTransfer"), msgNo_published(0)
{
    ui->setupUi(this);

    m_client = new QMqttClient(this);
    qDebug() << m_client->protocolVersion() << endl;

    connect(m_client, &QMqttClient::stateChanged, this, &MainWindow::updateLogStateChange);
    connect(m_client, &QMqttClient::connected, this, &MainWindow::subscribeFTPPort);
    connect(m_client, &QMqttClient::disconnected, this, &MainWindow::brokerDisconnected);
    connect(m_client, &QMqttClient::errorChanged, this, &MainWindow::errorHandler);
    connect(m_client, &QMqttClient::messageReceived, this, &MainWindow::messageHandler);

    connect(m_client, &QMqttClient::pingResponseReceived, this, [this]()
    {
        ui->buttonPing->setEnabled(true);
        writeLog(QLatin1String("PingResponse"));
    });

    m_client->setHostname(ui->lineEditHost->text());
    m_client->setPort(ui->spinBoxPort->value());
    m_client->setUsername(ui->lineEditUsername->text());
    m_client->setPassword(ui->lineEditPwd->text());
    connect(ui->lineEditHost, &QLineEdit::textChanged, m_client, &QMqttClient::setHostname);
    connect(ui->spinBoxPort, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::setClientPort);
    connect(ui->lineEditUsername, &QLineEdit::textChanged, m_client, &QMqttClient::setUsername);
    connect(ui->lineEditPwd, &QLineEdit::textChanged, m_client, &QMqttClient::setPassword);

    updateLogStateChange();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonConnect_clicked()
{
    if (m_client->state() == QMqttClient::Disconnected)
        m_client->connectToHost();
    else if (m_client->state() == QMqttClient::Connected)
        m_client->disconnectFromHost();
}

void MainWindow::on_buttonQuit_clicked()
{
    QApplication::quit();
}

void MainWindow::updateLogStateChange()
{
    writeLog(QLatin1String("State Change -> ") + ClientState2String[m_client->state()]);
}

void MainWindow::brokerDisconnected()
{
    ui->lineEditPwd->setEnabled(true);
    ui->lineEditUsername->setEnabled(true);
    ui->lineEditHost->setEnabled(true);
    ui->spinBoxPort->setEnabled(true);
    ui->buttonConnect->setText(tr("Connect"));
    ui->buttonPublish->setEnabled(false);
    ui->lineEditMessage->setEnabled(false);
}

void MainWindow::setClientPort(int p)
{
    m_client->setPort(p);
}

void MainWindow::on_buttonPublish_clicked()
{
    if (parseCommand(ui->lineEditMessage->text()) == WAITING)
        ui->buttonPublish->setEnabled(false);
}

void MainWindow::subscribeFTPPort()
{
    ui->lineEditHost->setEnabled(false);
    ui->spinBoxPort->setEnabled(false);
    ui->buttonConnect->setText(tr("Disconnect"));
    ui->lineEditPwd->setEnabled(false);
    ui->lineEditUsername->setEnabled(false);

    //    auto subscription = m_client->subscribe(ui->lineEditTopic->text());
    auto subscription1 = m_client->subscribe(topic_command_port, QoS2);
    auto subscription2 = m_client->subscribe(topic_dataTransfer_port, QoS2);

    if (subscription1 && subscription2)
    {
        ui->lineEditMessage->setEnabled(true);
        ui->buttonPublish->setEnabled(true);
        writeLog(QStringLiteral("Successfully subscribe topic: dataTransfer_port and topic: command_port!"));
        writeLog(QStringLiteral(u"当前本地文件夹为: ") + current_dir);
    }
    else
        QMessageBoxWrapper(QStringLiteral("Could not subscribe topic: command_port or topic: "
                                          "dataTransfer_port. Is there a valid connection?"));
}

void MainWindow::messageHandler(const QByteArray &message, const QMqttTopicName &topic)
{
    QDataStream qds(message);
    quint16 msgNo;
    qds >> msgNo;
    if (topic.name() == topic_command_port)
    {
        //        QDataStream qdin(message);
        //        quint32 msgNo;
        //        qdin >> msgNo;
        //        if (nowWaitingfor == msgNo) {

        //            QString lsResult;
        //            qdin >> lsResult;
        //            writeLog(std::move(lsResult));
        //        }
        qDebug() << QByteArray::number(msgNo_published) << endl;

        if (msgNo == msgNo_published)
        {
            writeLog(message.right(message.size() - 2));
            ui->buttonPublish->setEnabled(true);
        }
    }
    else if (topic.name() == topic_dataTransfer_port)
    {
        // 存文件
        qint64 fileNo = static_cast<qint64>(msgNo) - msgNo_beg_published;
        if (fileNo >= 0 && fileNo < files_to_get.size())
        {
            const auto &file_path = files_to_get[fileNo];
            QFileInfo file_info(file_path);
            QFile f(current_dir + QLatin1Char('/') + file_info.fileName());

            if (!f.open(QIODevice::WriteOnly))
                writeLog(QStringLiteral("failed to write the file: ") + file_path);
            else
            {
                writeLog(file_path + QStringLiteral(u"传输中..."));
                if (f.write(message.right(message.size() - 2)) == -1) //大小问题需解决，是否传成功
                    writeLog(QStringLiteral(u"文件传输失败"));
                else
                    writeLog(QStringLiteral(u"文件传输成功"));
                f.close();
            }
            ui->buttonPublish->setEnabled(true);
        }
    }
    else
        writeLog(QLatin1String("UnKnown Received Topic: ") + topic.name() + QLatin1String(" Message: ") + message);
}

void MainWindow::on_buttonPing_clicked()
{
    ui->buttonPing->setEnabled(false);
    m_client->requestPing();
}

void MainWindow::errorHandler(const QMqttClient::ClientError &err)
{
    switch (err)
    {
        case QMqttClient::BadUsernameOrPassword:
            QMessageBoxWrapper(QStringLiteral(u"用户不存在或密码错误"));
            break;
        case QMqttClient::InvalidProtocolVersion:
            QMessageBoxWrapper(QStringLiteral("Invalid Protocol Version"));
            break;
        case QMqttClient::IdRejected:
            QMessageBoxWrapper(QStringLiteral("Identifier rejected"));
            break;
        case QMqttClient::ServerUnavailable:
            QMessageBoxWrapper(QStringLiteral("Server Unavailable"));
            break;
        case QMqttClient::NoError:
            break;
        default:
            QMessageBoxWrapper(QStringLiteral("Unknown Error"));
            break;
    }
}

MainWindow::ParseState MainWindow::parseCommand(const QString &commandStr)
{
    auto cmdStr_trimmed = commandStr.trimmed();
    auto cmdStr_splitted = cmdStr_trimmed.split(QLatin1Char(' '), QString::SkipEmptyParts);

    if (cmdStr_splitted[0].compare(QLatin1String("put"), Qt::CaseInsensitive) == 0)
    {
        QStringList files = QFileDialog::getOpenFileNames(this, QStringLiteral(u"选取上传文件"), current_dir);
        return sendFiles(files);
    }
    else if (cmdStr_splitted[0].compare(QLatin1String("get"), Qt::CaseInsensitive) == 0)
    {
        //broker处理get
        files_to_get.clear();
        cmdStr_splitted.pop_front();
        files_to_get = std::move(cmdStr_splitted);
        return getFiles(cmdStr_trimmed);
    }
    else if (cmdStr_splitted[0].compare(QLatin1String("ls"), Qt::CaseInsensitive) == 0)
    {
        //broker处理list
        return listRemoteFiles(cmdStr_trimmed);
    }
//    else if (cmdStr_splitted[0].compare(QLatin1String("lcd"), Qt::CaseInsensitive) == 0)
//    {
//        current_dir = QFileDialog::getOpenFileName(this,
//                      QStringLiteral(u"选取本地文件夹"),
//                      current_dir,
//                      QString(),
//                      nullptr,
//                      QFileDialog::ShowDirsOnly);
//        writeLog(QStringLiteral(u"本地文件夹更改至: ") + current_dir);
//        return SUCCESS;
//    }
    else
    {
        writeLog(QStringLiteral("Command Parse Error!"));
        return FAILURE;
    }
}

MainWindow::ParseState MainWindow::sendFiles(const QStringList &file_list)
{
    for (auto &file_path : file_list)
    {
        QFile f(file_path);
        if (!f.open(QIODevice::ReadOnly))
        {
            writeLog(QStringLiteral("failed to read the file: ") + file_path);
            return FAILURE;
        }
        QByteArray byte_arr = f.readAll();
        writeLog(file_path + QStringLiteral(u"传输中..."));

        QFileInfo file_info(file_path);
        qDebug() << file_info.fileName() << endl;

        //信息格式：文件名+8个0+文件
        QByteArray msg_head = file_info.fileName().toUtf8().append(8, '\0');
        if (m_client->publish(topic_dataTransfer_port, byte_arr.prepend(msg_head), QoS2)
                == -1) //大小问题需解决，是否传成功
        {
            writeLog(QStringLiteral("Could not publish message"));
            return FAILURE;
        }
        else
            writeLog(QStringLiteral(u"文件传输成功"));
        f.close();
    }
    return SUCCESS;
}

MainWindow::ParseState MainWindow::getFiles(const QString &commandStr)
{
    const msgNoInt &files_count = static_cast<msgNoInt>(files_to_get.size());
    if (files_count == 0)
    {
        QMessageBoxWrapper(QStringLiteral("no file specified with 'get'"));
        return FAILURE;
    }
    // 每个文件都要有一个编码，第一个文件为msgNo_beg，后面每一个文件的msgNo依次加1
    msgNoInt msgNo_beg_tmp = std::uniform_int_distribution<msgNoInt>(0x1000, 0xFFFF - files_count + 1)(rd);
    QByteArray msg_head; // 格式为msgNo_beg--file_count--command
    QDataStream qds(&msg_head, QIODevice::WriteOnly);
    qds << msgNo_beg_tmp << files_count;
//    msg_head.setNum(msgNo_beg_tmp,16).append(QByteArray::number(files_count,16));
    assert(msg_head.size() == 4);
    if (m_client->publish(topic_command_port, commandStr.toUtf8().prepend(msg_head), QoS2)
            == -1) //大小问题需解决，是否传成功
    {
        writeLog(QStringLiteral("Could not publish message"));
        return FAILURE;
    }
    else
        msgNo_beg_published = std::move(msgNo_beg_tmp);
    return WAITING;
}

MainWindow::ParseState MainWindow::listRemoteFiles(const QString &commandStr)
{
    msgNoInt msgNo_tmp = std::uniform_int_distribution<msgNoInt>(0, 0x0FFF)(rd);
    QByteArray msg_head; // 格式为msgNo--command
    QDataStream qds(&msg_head, QIODevice::WriteOnly);
    qds << msgNo_tmp;
    assert(msg_head.size() == 2);
    if (m_client->publish(topic_command_port, commandStr.toUtf8().prepend(msg_head), QoS2)
            == -1) //大小问题需解决，是否传成功
    {
        writeLog(QStringLiteral("Could not publish message"));
        return FAILURE;
    }
    else
        msgNo_published = std::move(msgNo_tmp);
    return WAITING;
}

void MainWindow::QMessageBoxWrapper(const QString &&warning_message)
{
    QMessageBox::warning(this, QStringLiteral("Error"), warning_message);
}

void MainWindow::writeLog(const QString &&content)
{
    ui->textLog->appendPlainText(QDateTime::currentDateTime().toString() + QLatin1String(" : ") + content);
}
