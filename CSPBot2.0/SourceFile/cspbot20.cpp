#include "cspbot20.h"
#include "stdafx.h"
#include "helper.h"
#include "global.h"
#include "logger.h"
#include "websocketClient.h"
#include "regularEdit.h"

using namespace std;

///////////////////////////////////////////// Global /////////////////////////////////////////////
//�رն���Animation
QGraphicsOpacityEffect* c_pOpacity;
QPropertyAnimation* c_pAnimation;

///////////////////////////////////////////// Export /////////////////////////////////////////////
//�����������־
void CSPBot::insertLog(QString a) {;
    ui.botconsole->setReadOnly(false);
    ui.botconsole->append(a);
    ui.botconsole->setReadOnly(true);
};

bool connectMirai();

///////////////////////////////////////////// Main /////////////////////////////////////////////
CSPBot::CSPBot(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    
    //////// Style ////////
    //������ʽ
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    //�����ޱ߿�
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    //���ô�����Ӱ
    QGraphicsDropShadowEffect* shadow_effect = new QGraphicsDropShadowEffect(this);
    shadow_effect->setOffset(0, 0);
    shadow_effect->setColor(Qt::gray);
    shadow_effect->setBlurRadius(10);
    ui.background->setGraphicsEffect(shadow_effect);

    //������
    vector<QScrollBar*> bars = { ui.ServerLog->verticalScrollBar(),ui.botconsole->verticalScrollBar() };
    for (QScrollBar* bar : bars) {
        setAllScrollbar(bar);
    }

    //��Ӱ����
    vector<QWidget*> buttons = {
        ui.ServerLog,
        ui.controlWidget,
        ui.statusWidget,
        ui.playerAdmin,
        ui.regularAdmin,
        ui.pluginAdmin,
        ui.inputCmd,
        ui.runCmd
    };
    for (auto bt : buttons) {
        setGraphics(bt);
    }

    //��������
    c_pOpacity = new QGraphicsOpacityEffect(this);
    c_pAnimation = new QPropertyAnimation(this);

    //////// Basic ////////
    //�󶨰汾�ŵ���ǩ
    ui.version->setText("V" + Helper::stdString2QString(version));

    //////// Bind ////////
    //ע�Ტ��
    qRegisterMetaType<StringMap>("StringMap");
    //��ҳ��ť
    connect(ui.mainPage, SIGNAL(clicked()), this, SLOT(switchPage()));
    connect(ui.playerPage, SIGNAL(clicked()), this, SLOT(switchPage()));
    connect(ui.regularPage, SIGNAL(clicked()), this, SLOT(switchPage()));
    connect(ui.pluginPage, SIGNAL(clicked()), this, SLOT(switchPage()));
    connect(ui.logPage, SIGNAL(clicked()), this, SLOT(switchPage()));

    //�����԰�ť
    connect(ui.close, SIGNAL(clicked()), this, SLOT(on_actionClose_triggered()));
    connect(ui.min, SIGNAL(clicked()), this, SLOT(on_actionMinimize_triggered()));

    //���¼�
    connect(c_pAnimation, &QPropertyAnimation::finished, this, &CSPBot::close);
    connect(this,SIGNAL(signalStartServer()), this, SLOT(startServer()));
    
    //Server�ఴť
    connect(ui.start, SIGNAL(clicked()), this, SLOT(startServer()));
    connect(ui.stop, SIGNAL(clicked()), this, SLOT(stopServer()));
    connect(ui.forceStop, SIGNAL(clicked()), this, SLOT(forceStopServer()));
    connect(ui.clear, SIGNAL(clicked()), this, SLOT(clear_console()));
    connect(ui.ServerCmd, SIGNAL(clicked()), this, SLOT(startCmd())); //������cmd
    connect(ui.runCmd, SIGNAL(clicked()), this, SLOT(insertCmd())); //����������

    //�󶨿�ݼ�
    connect(this, SIGNAL(runCommand()), this, SLOT(insertCmd())); //�󶨻س���������
    connect(this, SIGNAL(runCmd()), ui.ServerCmd, SLOT(click())); //������cmd


    //////// Mirai ////////
    mirai = new Mirai();
    connect(mirai, SIGNAL(setUserImages(QString, QString)), this, SLOT(setUserImage(QString, QString)));


    //////// Logger ////////
    LoggerReader* loggerReader = new LoggerReader(this);
    connect(loggerReader, SIGNAL(updateLog(QString)), this, SLOT(insertLog(QString)));
    loggerReader->start();
    connectMirai();

    /////// Other /////////
    ui.inputCmd->setEnabled(false);
    ui.runCmd->setEnabled(false);
    ui.ServerLog->setEnabled(false);

    /////// Test ////////
    Regular re = { "","",Console,group,true };
    regularEdit* resub = new regularEdit(re,this);
    resub->show();

}

///////////////////////////////////////////// Style /////////////////////////////////////////////
void CSPBot::setGraphics(QWidget* bt) {
    QGraphicsDropShadowEffect* shadow_effect = new QGraphicsDropShadowEffect(this);
    shadow_effect->setOffset(0, 0);
    shadow_effect->setColor(Qt::gray);
    shadow_effect->setBlurRadius(8);
    bt->setGraphicsEffect(shadow_effect);
}

void CSPBot::setAllScrollbar(QScrollBar* bar) {
    bar->setStyleSheet("QScrollBar:vertical"
        "{"
        "width:8px;"
        "background:rgba(0,0,0,0%);"
        "margin:0px,0px,0px,0px;"
        "padding-top:9px;"
        "padding-bottom:9px;"
        "}"
        "QScrollBar::handle:vertical"
        "{"
        "width:8px;"
        "background:rgba(0,0,0,25%);"
        " border-radius:4px;"
        "min-height:20;"
        "}"
        "QScrollBar::handle:vertical:hover"
        "{"
        "width:8px;"
        "background:rgba(0,0,0,50%);"
        " border-radius:4px;"
        "min-height:20;"
        "}"
        "QScrollBar::add-line:vertical"
        "{"
        "height:9px;width:8px;"
        "border-image:url(:/a/Images/a/3.png);"
        "subcontrol-position:bottom;"
        "}"
        "QScrollBar::sub-line:vertical"
        "{"
        "height:9px;width:8px;"
        "border-image:url(:/a/Images/a/1.png);"
        "subcontrol-position:top;"
        "}"
        "QScrollBar::add-line:vertical:hover"
        "{"
        "height:9px;width:8px;"
        "border-image:url(:/a/Images/a/4.png);"
        "subcontrol-position:bottom;"
        "}"
        "QScrollBar::sub-line:vertical:hover"
        "{"
        "height:9px;width:8px;"
        "border-image:url(:/a/Images/a/2.png);"
        "subcontrol-position:top;"
        "}"
        "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
        "{"
        "background:rgba(0,0,0,10%);"
        "border-radius:4px;"
        "}"
    );
}

QPixmap PixmapToRound(QPixmap& src, int radius);

void CSPBot::setUserImage(QString qqNum, QString qqNick) {
    QString szUrl = "https://q.qlogo.cn/g?b=qq&nk=" + qqNum + "&s=640";
    QUrl url(szUrl);
    QNetworkAccessManager manager;
    QEventLoop loop;
    qDebug() << "Reading picture form " << url;
    QNetworkReply* reply = manager.get(QNetworkRequest(url));
    //���������������ɺ��˳����¼�ѭ��
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    //�������¼�ѭ��
    loop.exec();
    QByteArray jpegData = reply->readAll();
    QPixmap pixmap;
    pixmap.loadFromData(jpegData);
    ui.userImage->setPixmap(PixmapToRound(pixmap, 45)); // ����QLabel��ʾͼƬ
    ui.user->setText(qqNick);
}


QPixmap PixmapToRound(QPixmap& src, int radius)
{
    if (src.isNull()) {
        return QPixmap();
    }

    QSize size(src.width(), src.height());
    QSize size2(radius * 2, radius * 2);
    QBitmap mask(size);
    QPainter painter(&mask);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);//�����
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setRenderHints(QPainter::TextAntialiasing);
    painter.translate(0, 0);
    //painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(0, 0, size.width(), size.height(), Qt::white);
    painter.setBrush(QColor(0, 0, 0));
    painter.drawEllipse(0, 0, size.width(), size.height());
    src.setMask(mask);
    QPixmap image = src.scaled(size2);
    return image;
}

///////////////////////////////////////////// Basic /////////////////////////////////////////////
void CSPBot::switchPage() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());//�õ����µİ�ť��ָ��
    QPushButton* btns[5] = { ui.mainPage,ui.playerPage,ui.pluginPage,ui.regularPage,ui.logPage };
    for (auto i : btns) {
        i->setStyleSheet("QPushButton{background-color:#f0f3f8;color:#666666;border-radius:10px;qproperty-iconSize: 32px 32px;}QPushButton:hover{background-color:#e0e6ee;border-radius:10px;};");
    }
    if (button == ui.mainPage) {
        ui.main->setCurrentIndex(0);
    }
    else if (button == ui.playerPage) {
        ui.main->setCurrentIndex(1);
    }
    else if (button == ui.regularPage) {
        ui.main->setCurrentIndex(2);
    }
    else if (button == ui.pluginPage) {
        ui.main->setCurrentIndex(3);
    }
    else if (button == ui.logPage) {
        ui.main->setCurrentIndex(4);
    }
    button->setStyleSheet("background-color:#ccdff8;\ncolor:#666666;\nborder-radius:10px;\nqproperty-iconSize: 32px 32px;\n");
}


///////////////////////////////////////////// MoveWindow /////////////////////////////////////////////
//�������ĳ���
const static int pos_min_x = 0;
const static int pos_max_x = 1010;
const static int pos_min_y = 0;
const static int pos_max_y = 30;
//�Լ�ʵ�ֵĴ����϶�����
void CSPBot::mousePressEvent(QMouseEvent* event)
{
    mousePosition = event->pos();
    //ֻ�Ա�������Χ�ڵ�����¼����д���
    if (mousePosition.x() <= pos_min_x)
        return;
    if (mousePosition.x() >= pos_max_x)
        return;
    if (mousePosition.y() <= pos_min_y)
        return;
    if (mousePosition.y() >= pos_max_y)
        return;
    isMousePressed = true;
}
void CSPBot::mouseMoveEvent(QMouseEvent* event)
{
    if (isMousePressed == true)
    {
        QPoint movePot = event->globalPos() - mousePosition;
        move(movePot);
    }
}
void CSPBot::mouseReleaseEvent(QMouseEvent* event)
{
    isMousePressed = false;
}

void CSPBot::on_actionMinimize_triggered()
{
    showMinimized();
    //��� showMaximized()��
}
void CSPBot::on_actionClose_triggered()
{
    if (checkClose()) {
        c_pAnimation->setTargetObject(this);
        c_pOpacity->setOpacity(0);
        setGraphicsEffect(c_pOpacity);
        c_pAnimation->setTargetObject(c_pOpacity);
        c_pAnimation->setPropertyName("opacity");
        c_pAnimation->setStartValue(1);
        c_pAnimation->setEndValue(0);
        c_pAnimation->start();
    }
}

bool CSPBot::checkClose() {
    if (server != nullptr && server->started) {
        auto temp = QMessageBox::warning(
            this,
            u8"����",
            u8"�������������У����Ƿ�Ҫ�ر�?",
            QMessageBox::Yes | QMessageBox::No
        );
        if (temp == QMessageBox::Yes)
        {
            server->forceStopServer();
            /*Callbacker cbe(EventCode::onStop);
            if (cbe.callback()) {
                event->accept();
            }
            else {
                event->ignore();
            }*/
        }
        else
        {
            return false;
        }
    }
    return true;
}

///////////////////////////////////////////// Server /////////////////////////////////////////////
//����Server
void CSPBot::buildServer(int mode) {
    server = new Server(mode,this);
    //�󶨼����
    connect(server, SIGNAL(insertBDSLog(QString)), this, SLOT(slotInsertBDSLog(QString)));
    connect(server, SIGNAL(OtherCallback(QString,StringMap)), this, SLOT(slotOtherCallback(QString, StringMap)));
    connect(server, SIGNAL(chenableForce(bool)), this, SLOT(slotChenableForce(bool)));
    connect(server, SIGNAL(chLabel(QString, QString)), this, SLOT(slotChLabel(QString, QString)));
    connect(server, SIGNAL(changeStatus(bool)), this, SLOT(slotChangeStatus(bool)));
    //����Server�߳�
    server->start();
}

//����BDS��־
void CSPBot::slotInsertBDSLog(QString log) {
    ui.ServerLog->setReadOnly(false);
    ui.ServerLog->append(log);
    ui.ServerLog->setReadOnly(true);
}; 

//Callback
void CSPBot::slotOtherCallback(QString listener, StringMap args) {
    
}; 

//����״̬
void CSPBot::slotChangeStatus(bool a) {
    if (a) {
        ui.ServerStatus->setText(u8"״̬: ������");
    }
    else {
        ui.ServerStatus->setText(u8"״̬: δ����");
    }
}; 

//���ı�ǩ
void CSPBot::slotChLabel(QString title, QString content) {
    if (title == "world") {
        ui.ServerWorld->setText(u8"����:" + content);
    }
    else if (title == "version") {
        ui.ServerVersion->setText(u8"�汾:" + content);
    }
    else if (title == "difficult") {
        ui.ServerDifficult->setText(u8"�Ѷ�:" + content);
    }
}; 

//����ǿ��ֹͣ״̬
void CSPBot::slotChenableForce(bool a) {
    ui.forceStop->setEnabled(a);
    if (a) {
        /*ui.change->setText(u8"ֹͣ");*/
        ui.stop->setEnabled(true);
        ui.start->setEnabled(false);
        ui.inputCmd->setEnabled(true);
        ui.runCmd->setEnabled(true);
    }
    else {
        /*ui.change->setText(u8"����");*/
        ui.stop->setEnabled(false);
        ui.start->setEnabled(true);
        ui.inputCmd->setEnabled(false);
        ui.runCmd->setEnabled(false);
    }
}; 

//����������
void CSPBot::startServer() {
    buildServer(0);
    ui.stop->setEnabled(true);
    slotInsertBDSLog(u8"[CSPBot] ������̷�����������");
    ui.start->setEnabled(false);
}

//����Cmd
void CSPBot::startCmd() {
    buildServer(1);
    ui.stop->setEnabled(true);
    slotInsertBDSLog(u8"<font style=\"color:#FFCC66;\">!!![WARNNING] [CSPBot] ���ѽ���<font color=\"#008000\">CMD</font>����ģʽ. [WARNNING]!!!</font>");
    slotInsertBDSLog(u8"[CSPBot] ������̷�����������");
    ui.start->setEnabled(false);
}

//�رշ�����
void CSPBot::stopServer() {
    server->stopServer();
    slotInsertBDSLog(u8"[CSPBot] ������̷�����ֹ����");
    ui.stop->setEnabled(false);
    ui.start->setEnabled(true);
}

//ǿ��ֹͣ����
void CSPBot::forceStopServer() {
    auto temp = QMessageBox::warning(this, u8"����", u8"�������������У����Ƿ�Ҫǿ��ֹͣ?", QMessageBox::Yes | QMessageBox::No);
    if (temp == QMessageBox::Yes)
    {
        server->forceStopServer();
        ui.ServerLog->append(u8"[CSPBot] ������̷���ǿ����ֹ����");
    }
}

//�����־
void CSPBot::clear_console() {
    ui.ServerLog->setText("");
}

//��������
void CSPBot::insertCmd() {
    try {
        string cmd = Helper::QString2stdString(ui.inputCmd->text());
        if (cmd != "") {
            ui.inputCmd->setText("");
            server->sendCmd(cmd + "\n");
        }
    }
    catch (...) {

    }
}

///////////////////////////////////////////// Keyboard /////////////////////////////////////////////
void CSPBot::keyPressEvent(QKeyEvent* e)
{
    if (e->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier) && e->key() == Qt::Key_C)
    {
        emit runCmd();
    }
    else if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        emit runCommand();
    }
}