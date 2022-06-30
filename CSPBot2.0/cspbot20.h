#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_cspbot20.h"
#include <string>
#include "helper.h"
#include <QQueue>


class CSPBot : public QMainWindow
{
    Q_OBJECT

///////////////////////////////////////////// Public /////////////////////////////////////////////
public:
    CSPBot(QWidget *parent = Q_NULLPTR);

    //CallBack
    inline void OtherCallback(std::string name, std::unordered_map<std::string, std::string> a = {}) {};
    inline void chenableForce(bool a) {};
    inline void ipipelog(std::string a) {};
    
    inline void PacketCallback(std::string a) {};
    inline void chLabel(std::string a, std::string b) {};
    inline void CommandCallback(std::string a, std::vector<std::string> b) {};
    //���ź�ʹ��
    inline void changeStatus(bool a) {};

///////////////////////////////////////////// Signals /////////////////////////////////////////////
signals:
    void ilog(); //Logger ���ź�

///////////////////////////////////////////// Slot /////////////////////////////////////////////
private slots:
    void on_actionMinimize_triggered();//��С������
    void on_actionClose_triggered();//�رմ���
    void switchPage();//�л�����
    void insertLog(QString a);//Logger �ۺ���
    void setUserImage(QString qqNum, QString qqNick); //����ͷ��

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
    void closeEvent(QCloseEvent* event);
    QPoint mousePosition;
    bool isMousePressed;
    int startCount = 0;

    //////// UI ////////
    Ui::Form ui;
};
