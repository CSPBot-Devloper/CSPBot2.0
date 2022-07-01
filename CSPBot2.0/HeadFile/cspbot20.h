#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_cspbot20.h"
#include <string>
#include "helper.h"
#include <QQueue>

//Type
typedef std::unordered_map<std::string, std::string> StringMap;

class CSPBot : public QMainWindow
{
    Q_OBJECT

///////////////////////////////////////////// Public /////////////////////////////////////////////
public:
    CSPBot(QWidget *parent = Q_NULLPTR);


///////////////////////////////////////////// Signals /////////////////////////////////////////////
signals:
    void ilog(); //Logger ���ź�
    void runCmd(); //����Cmd
    void runCommand(); //��������
    void signalStartServer(); //����������

///////////////////////////////////////////// Slot /////////////////////////////////////////////
private slots:
    void on_actionMinimize_triggered();//��С������
    void on_actionClose_triggered();//�رմ���
    void switchPage();//�л�����
    void insertLog(QString a);//Logger �ۺ���
    void setUserImage(QString qqNum, QString qqNick); //����ͷ��

    /////////////// Server //////////////////
    //ServerRebackSlot
    void slotOtherCallback(QString listener, StringMap args);
    void slotChenableForce(bool a);
    void slotInsertBDSLog(QString log);
    void slotChangeStatus(bool a);
    void slotChLabel(QString title, QString content);
    //ServerMainSlot
    void startServer(); //����������
    void stopServer(); //ֹͣ������
    void forceStopServer(); //ǿ��ֹͣ������
    void clear_console(); //���BDS��־
    void startCmd(); //����cmd
    void insertCmd(); //��������
    //void PacketCallback(std::string a);
    //void CommandCallback(std::string a, std::vector<std::string> b);
    

///////////////////////////////////////////// Private /////////////////////////////////////////////
private:
    //////// Basic ////////
    //�����Ӱ
    void setGraphics(QWidget* bt);

    //////// Window ////////
    void mouseMoveEvent(QMouseEvent* event);
    void setAllScrollbar(QScrollBar* bar);
    //void setGraphics(QWidget* bt);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent*);
    //�ر��¼�
    bool checkClose();
    QPoint mousePosition;
    bool isMousePressed;
    int startCount = 0;

    //////// Server ////////
    void buildServer(int mode=0);


    /////// Keyboard ///////
    void keyPressEvent(QKeyEvent* e);

    //////// UI ////////
    Ui::Form ui;
};
