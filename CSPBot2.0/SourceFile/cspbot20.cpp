#include "cspbot20.h"
#include "stdafx.h"
#include "helper.h"
#include "global.h"
#include "logger.h"
#include "websocketClient.h"
#include "regularEdit.h"
#include "pluginModule.h"


using namespace std;

///////////////////////////////////////////// Global /////////////////////////////////////////////
//�رն���Animation
QGraphicsOpacityEffect* c_pOpacity;
QPropertyAnimation* c_pAnimation;
//Mirai��¼����
bool connectMirai();

///////////////////////////////////////////// Export /////////////////////////////////////////////
//�����������־
void CSPBot::insertLog(QString a) {;
    ui.botconsole->setReadOnly(false);
    ui.botconsole->append(a);
    ui.botconsole->moveCursor(QTextCursor::End);
    ui.botconsole->setReadOnly(true);
};

//������������־
void CSPBot::startLogger() {
    //////// Logger ////////
    LoggerReader* loggerReader = new LoggerReader(this);
    connect(loggerReader, SIGNAL(updateLog(QString)), this, SLOT(insertLog(QString)));
    loggerReader->start();
}

//�������̨��־
void CSPBot::slotSaveConsole() {
    if (ui.botconsole->toPlainText() == "") {
        QMessageBox::information(this, u8"��ʾ", u8"����̨��־Ϊ��",
            QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this,
        tr(u8"���浱ǰ��־"),
        "",
        tr(u8"��־�ļ�(*.txt)"));
    if (fileName == "") {
        return;
    }
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, u8"���ش���", u8"�ļ�����ʧ�ܣ�",
            QMessageBox::Yes, QMessageBox::Yes);
    }
    else
    {
        QTextStream stream(&file);
        stream << ui.botconsole->toPlainText();
        stream.flush();
        file.close();
    }
}

//��տ���̨��־
void CSPBot::slotClearConsole() {
    ui.botconsole->setText("");
}

//����������
void CSPBot::slotUpdateSendRecive(int send, int recive) {
    string sendString = to_string(send);
    string reciveString = to_string(recive);
    if (send > 99) {
        sendString = "99+";
    }
    if (recive > 99) {
        reciveString = "99+";
    }
    string reFormat = fmt::format(u8"��Ϣ:{}��;{}��", sendString,reciveString);
    ui.websocketMsg->setText(Helper::stdString2QString(reFormat));
}

//��������ʱ��
void CSPBot::slotConnected(mTime getTime) {
    mGetTime = getTime;
}

//�ֶ�����Mirai
void CSPBot::slotConnectMirai() {
    if (mirai->logined == false) {
        string formatLog = fmt::format(u8"<font color=\"#FFCC66\">{} W/Mirai: {}\n</font>", Logger::getTime(), u8"��������Mirai...");
        insertLog(Helper::stdString2QString(formatLog));
        connectMirai();
    }
    else {
        string formatLog = fmt::format(u8"<font color=\"#FFCC66\">{} W/Mirai: {}\n</font>", Logger::getTime(), u8"�����Ѵ���������״̬.");
        insertLog(Helper::stdString2QString(formatLog));
    }
}

//�ֶ��Ͽ�Mirai
void CSPBot::slotDisConnectMirai() {
    if (mirai->logined == false) {
        
        string formatLog = fmt::format(u8"<font color=\"#FFCC66\">{} W/Mirai: {}\n</font>", Logger::getTime(), u8"����δ����������״̬.");
        insertLog(Helper::stdString2QString(formatLog));
    }
    else {
        string formatLog = fmt::format(u8"<font color=\"#FFCC66\">{} W/Mirai: {}\n</font>", Logger::getTime(), u8"���ڶϿ�Mirai...");
        insertLog(Helper::stdString2QString(formatLog));
        wsc->shutdown();
    }
}

///////////////////////////////////////////// Timer /////////////////////////////////////////////
void CSPBot::slotTimerFunc() {
    //////// Connect ////////
    mTime nowTime = time(NULL);
    int min;
    string minString;
    if (mGetTime == 0) {
        min = 0;
    }
    else {
        int f = nowTime - mGetTime;
        if (f < 60) {
            min = 0;
        }
        else {
            min = f/60;
        }
        
    }
    minString = to_string(min);
    if (min > 99) {
        minString = "99+";
    }
    string minFormat = fmt::format(u8"����ʱ��:{}m", minString);
    ui.websocketConnectedTime->setText(Helper::stdString2QString(minFormat));

    //////// Mirai ////////
    if (mirai->logined) {
        ui.websocketStatus->setText(u8"״̬: ������");
    }
    else {
        ui.websocketStatus->setText(u8"״̬: δ����");
    }

    /////// Table ////////
    InitPlayerTableView();
    InitRegularTableView();
    InitPluginTableView();
}


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
    this->setWindowTitle(u8"CSPBot v" + Helper::stdString2QString(version));
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
        ui.runCmd,
        ui.consoleWidget,
        ui.websocketWidget,
        ui.regularWidget,
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
    qRegisterMetaType<mTime>("mTime");
    qRegisterMetaType<StringVector>("StringVector");

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
    connect(this, SIGNAL(signalStartLogger()), this, SLOT(startLogger())); //����Logger����

    //�󶨿�ݼ�
    connect(this, SIGNAL(runCommand()), this, SLOT(insertCmd())); //�󶨻س���������
    connect(this, SIGNAL(runCmd()), ui.ServerCmd, SLOT(click())); //������cmd

    //������Console
    connect(ui.consoleSave, SIGNAL(clicked()), this, SLOT(slotSaveConsole())); //������־
    connect(ui.consoleClear, SIGNAL(clicked()), this, SLOT(slotClearConsole())); //��տ���̨
    connect(ui.connect, SIGNAL(clicked()), this, SLOT(slotConnectMirai())); //����Mirai
    connect(ui.disConnect, SIGNAL(clicked()), this, SLOT(slotDisConnectMirai())); //�Ͽ�����

    //���
    connect(ui.regularAdmin, SIGNAL(pressed(QModelIndex)), this, SLOT(clickRegularTable(QModelIndex)));
    connect(ui.regularAdmin, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClickedRegularTable(QModelIndex)));
    connect(ui.regularNew, SIGNAL(clicked()), this, SLOT(newRegular())); //�½�����ť

    //////// Mirai ////////
    mirai = new Mirai();
    connect(mirai, SIGNAL(setUserImages(QString, QString)), this, SLOT(setUserImage(QString, QString)));
    connect(mirai, SIGNAL(updateSendRecive(int, int)), this, SLOT(slotUpdateSendRecive(int, int)));
    connect(mirai, SIGNAL(signalConnect(mTime)), this, SLOT(slotConnected(mTime)));
    connect(mirai, 
        SIGNAL(OtherCallback(QString, StringMap)), 
        this, 
        SLOT(slotOtherCallback(QString, StringMap))
        ,Qt::BlockingQueuedConnection
    );
    connectMirai();

    /////// Other /////////
    ui.inputCmd->setEnabled(false);
    ui.runCmd->setEnabled(false);
    ui.stop->setEnabled(false);
    ui.forceStop->setEnabled(false);
    ui.ServerLog->setReadOnly(true);
    ui.botconsole->setReadOnly(true);
    commandApi = new CommandAPI();
    connect(commandApi, SIGNAL(signalStartServer()), this, SLOT(startServer()));
    connect(commandApi, SIGNAL(signalCommandCallback(QString,StringVector)), this, SLOT(slotCommandCallback(QString,StringVector)));

    /////// timer /////////
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerFunc()));
    timer->start(1 * 1000);

    /////// Table /////////
    InitPlayerTableView();
    InitRegularTableView();
    InitPluginTableView();
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

void CSPBot::setUserImageError(QNetworkReply::NetworkError e) {
    auto error = magic_enum::enum_name<QNetworkReply::NetworkError>(e);
    Logger lo("Mirai");
    lo.error("{}", error);
}

void CSPBot::setUserImage(QString qqNum, QString qqNick) {
    if (qqNum == "" || qqNick == "") {
        QPixmap pixmap = QPixmap();
        ui.userImage->setPixmap(pixmap);
        ui.user->setText(u8"�û�δ��¼");
        ui.userImage->setText(u8"�û�δ��¼");
        return;
    }
    QString szUrl = "https://q.qlogo.cn/g?b=qq&nk=" + qqNum + "&s=640";
    QUrl url(szUrl);
    QNetworkAccessManager manager;
    QEventLoop loop;
    qDebug() << "Reading picture form " << url;
    QNetworkReply* reply = manager.get(QNetworkRequest(url));
    //���������������ɺ��˳����¼�ѭ��
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(setUserImageError(QNetworkReply::NetworkError)));
    //�������¼�ѭ��
    loop.exec();
    QByteArray jpegData = reply->readAll();
    QPixmap pixmap;
    pixmap.loadFromData(jpegData);
    if (!pixmap.isNull()) {
        ui.userImage->setPixmap(PixmapToRound(pixmap, 45)); // ����QLabel��ʾͼƬ
    }
    else {
        ui.userImage->setText(u8"ͼƬ���ش���");
    }
    
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
            Callbacker cbe(EventCode::onStop);
            if (cbe.callback()) {
                return true;
            }
            else {
                return false;
            }
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
	ui.ServerLog->moveCursor(QTextCursor::End);
    ui.ServerLog->setReadOnly(true);
}; 

//Callback
bool CSPBot::slotOtherCallback(QString listener, StringMap args) {
    qDebug() << "CallBack:" << listener;
    string eventName = Helper::QString2stdString(listener);
    auto event_code = magic_enum::enum_cast<EventCode>(eventName.c_str());
    if (!event_code) {
        return false;
    }
    EventCode ct = event_code.value();
    Callbacker cb(ct);
    for(auto& i:args) {
        string key = i.first;
        string value = i.second;
        cb.insert(key.c_str(), py::str(value));
    }

    return cb.callback();
}; 

void CSPBot::slotCommandCallback(QString cmd,StringVector fArgs){
    string type = Helper::QString2stdString(cmd);
    if (command.find(type) != command.end()) {
        py::list args;
        for (auto& i:fArgs) {
            args.append(py::str(i));
        }
        py::function cbe = command[type];
        cbe(args);
    }
}

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

///////////////////////////////////////////// Table /////////////////////////////////////////////
void CSPBot::InitPlayerTableView()
{
    try {
        YAML::Node node = YAML::LoadFile("data/player.yml"); //��ȡplayer�����ļ�
        int line_num = node.size();
        QStringList strHeader;
        strHeader << u8"�������" << u8"���Xuid" << u8"���QQ��";

        QStandardItemModel* m_model = new QStandardItemModel();
        m_model->setHorizontalHeaderLabels(strHeader);
        m_model->setColumnCount(strHeader.size());
        m_model->setRowCount(line_num);
        ui.playerAdmin->verticalHeader()->hide();
        ui.playerAdmin->setModel(m_model);

        //������ʾ�������ı�
        int in = 0;
        for (YAML::Node i : node)
        {
            string playerName = i["playerName"].as<string>();
            string xuid = i["xuid"].as<string>();
            string qq = i["qq"].as<string>();
            QStandardItem* item1 = new QStandardItem(Helper::stdString2QString(playerName));
            item1->setTextAlignment(Qt::AlignCenter);
            QStandardItem* item2 = new QStandardItem(Helper::stdString2QString(xuid));
            item2->setTextAlignment(Qt::AlignCenter);
            QStandardItem* item3 = new QStandardItem(Helper::stdString2QString(qq));
            item3->setTextAlignment(Qt::AlignCenter);
            m_model->setItem(in, 0, item1);
            m_model->setItem(in, 1, item2);
            m_model->setItem(in, 2, item3);
            in++;
        }
        ui.playerAdmin->horizontalHeader()->setStretchLastSection(true);
        ui.playerAdmin->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui.playerAdmin->setEditTriggers(QAbstractItemView::NoEditTriggers);
        //ui.playerAdmin->setFrameShape(QListWidget::NoFrame);
        ui.playerAdmin->setAlternatingRowColors(true);
        ui.playerAdmin->setShowGrid(false);
    }
    catch (...) {

    }
}

void CSPBot::InitRegularTableView()
{
    try {
        YAML::Node node = YAML::LoadFile("data/regular.yml"); //��ȡplayer�����ļ�
        int line_num = node.size();
        QStringList strHeader;
        strHeader << u8"����" << u8"��Դ" << u8"ִ��" << u8"Ȩ��";

        QStandardItemModel* m_model = new QStandardItemModel();
        //��ӱ�ͷ����
        m_model->setHorizontalHeaderLabels(strHeader);
        //��������
        m_model->setColumnCount(strHeader.size());
        //��������
        m_model->setRowCount(line_num);

        //�����б�ͷ
        ui.regularAdmin->verticalHeader()->hide();

        //setModel.
        ui.regularAdmin->setModel(m_model);
        //������ʾ�������ı�
        int in = 0;
        for (YAML::Node i : node)
        {
            string Regular = i["Regular"].as<string>();
            string Action = i["Action"].as<string>();
            string From = i["From"].as<string>();
            string Permissions;
            if (i["Permissions"].as<bool>()) {
                Permissions = u8"��";
            }
            else if (!i["Permissions"].as<bool>()) {
                Permissions = u8"��";
            }
            QStandardItem* item1 = new QStandardItem(Helper::stdString2QString(Regular));
            item1->setTextAlignment(Qt::AlignCenter);
            QStandardItem* item2 = new QStandardItem(Helper::stdString2QString(From));
            item2->setTextAlignment(Qt::AlignCenter);
            QStandardItem* item3 = new QStandardItem(Helper::stdString2QString(Action));
            item3->setTextAlignment(Qt::AlignCenter);
            QStandardItem* item4 = new QStandardItem(Helper::stdString2QString(Permissions));
            item4->setTextAlignment(Qt::AlignCenter);
            m_model->setItem(in, 0, item1);
            m_model->setItem(in, 1, item2);
            m_model->setItem(in, 2, item3);
            m_model->setItem(in, 3, item4);
            in++;
        }
        ui.regularAdmin->horizontalHeader()->setStretchLastSection(true);
        ui.regularAdmin->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui.regularAdmin->setEditTriggers(QAbstractItemView::NoEditTriggers);
        //ui.playerAdmin->setFrameShape(QListWidget::NoFrame);
        ui.regularAdmin->setAlternatingRowColors(true);
        ui.regularAdmin->setShowGrid(false);
        
    }
    catch (...) {

    }
}

//ѡ���Զ�ѡ�и���
void CSPBot::clickRegularTable(QModelIndex index) {
    ui.regularAdmin->selectRow(index.row());
}

void CSPBot::doubleClickedRegularTable(QModelIndex index) {
    int row = ui.regularAdmin->currentIndex().row();
    auto modelViewOneUp = ui.regularAdmin->model();
    vector<string> regularData;
    /*
    enum regularAction { Console, Group, Command };
    enum regularFrom { group, console };
    struct Regular {
	    QString regular;
	    std::string action;
	    regularAction type;
	    regularFrom from;
	    bool permission;
    };
    strHeader << u8"����" << u8"��Դ" << u8"ִ��" << u8"Ȩ��";
    */
    for (int i=0; i<5; i++)
    {
        QModelIndex index = modelViewOneUp->index(row, i);
        QString name = modelViewOneUp->data(index).toString();
        regularData.push_back(Helper::QString2stdString(name));
    }
    string mRegular = regularData[0];
    string mFrom = regularData[1];
    string mAction = regularData[2];
    string mPermission = regularData[3];

    //ת��type
    string Action_type = mAction.substr(0, 2);
    regularAction regular_action;
    if (Action_type == "<<") { 
        regular_action = regularAction::Console; 
    }
    else if (Action_type == ">>") { 
        regular_action = regularAction::Group; 
    }
    else { regular_action = regularAction::Command; };

    //ת��Ȩ��
    bool Permission = (mPermission == u8"��");

    //ת����Դ
    regularFrom regular_from;
    transform(mFrom.begin(), mFrom.end(), mFrom.begin(), ::tolower);
    if (mFrom == "group") { 
        regular_from = regularFrom::group;
    }
    else { 
        regular_from = regularFrom::console;
    };

    Regular regular = { 
        Helper::stdString2QString(regularData[0]),
        Helper::stdString2QString(regularData[2]),
        regular_action,
        regular_from,
        Permission};
    regularEdit* regEdit = new regularEdit(regular,false,this);
    regEdit->show();
}

//�½�����
void CSPBot::newRegular() {
    Regular regular = {
        "",
        "",
        Console,
        console,
        false };
    regularEdit* regEdit = new regularEdit(regular,true,this);
    regEdit->show();
}

void CSPBot::InitPluginTableView()
{
    try {
        int line_num = plugins.size();
        QStringList strHeader;
        strHeader << u8"�ļ���" << u8"���" << u8"����" << u8"�汾" << u8"����";

        QStandardItemModel* m_model = new QStandardItemModel();
        m_model->setHorizontalHeaderLabels(strHeader);
        m_model->setColumnCount(strHeader.size());
        m_model->setRowCount(line_num);
        ui.pluginAdmin->verticalHeader()->hide();
        ui.pluginAdmin->setModel(m_model);
        int in = 0;
        for (auto& i : plugins)
        {
            QStandardItem* item1 = new QStandardItem(Helper::stdString2QString(i.first));
            QStandardItem* item2 = new QStandardItem(Helper::stdString2QString(i.second.name));
            QStandardItem* item3 = new QStandardItem(Helper::stdString2QString(i.second.info));
            QStandardItem* item4 = new QStandardItem(Helper::stdString2QString(i.second.version));
            QStandardItem* item5 = new QStandardItem(Helper::stdString2QString(i.second.author));

            //�����ı�
            item1->setTextAlignment(Qt::AlignCenter);
            item2->setTextAlignment(Qt::AlignCenter);
            item3->setTextAlignment(Qt::AlignCenter);
            item4->setTextAlignment(Qt::AlignCenter);
            item5->setTextAlignment(Qt::AlignCenter);

            m_model->setItem(in, 0, item1);
            m_model->setItem(in, 1, item2);
            m_model->setItem(in, 2, item3);
            m_model->setItem(in, 3, item4);
            m_model->setItem(in, 4, item5);
            in++;
        }
        ui.pluginAdmin->horizontalHeader()->setStretchLastSection(true);
        ui.pluginAdmin->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui.pluginAdmin->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui.pluginAdmin->setAlternatingRowColors(true);
        ui.pluginAdmin->setShowGrid(false);

    }
    catch (...) {

    }
}
