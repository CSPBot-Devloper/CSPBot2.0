#include "regularEdit.h"
#include <QGraphicsDropShadowEffect>
#include <qevent.h>
#include "qdebug.h"
#include "qprocess.h"
#include <QtCore/qpropertyanimation.h>
#include <QtWidgets/qmessagebox.h>

using namespace std;
QPropertyAnimation* m_pAnimation = new QPropertyAnimation();
QGraphicsOpacityEffect* m_pOpacity = new QGraphicsOpacityEffect();

regularEdit::regularEdit(Regular regular,bool newRegular, QWidget* parent)
    : QWidget(parent)
{
    regular_ui.setupUi(this);
    mRegular = regular;
    mNewRegular = newRegular;

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
    regular_ui.background->setGraphicsEffect(shadow_effect);

    vector<QWidget*> widget = {
        regular_ui.save,
        regular_ui.cancel,
        regular_ui.regularTest,
        regular_ui.remove,
    };
    for (QWidget* i : widget) {
        setGraphics(i);
    }


    //����
    groupButton1 = new QButtonGroup(this);
    groupButton1->addButton(regular_ui.console, 0);
    groupButton1->addButton(regular_ui.group, 1);
    //ui->apple_radioButton->setChecked(true);
    setRegular();
    if (mNewRegular) {
        regular_ui.remove->setEnabled(false);
    }

    ///////// Bind //////////
    connect(regular_ui.regularTest, SIGNAL(clicked()), this, SLOT(stratProgress()));
    connect(regular_ui.close, SIGNAL(clicked()), this, SLOT(on_actionClose_triggered()));
    connect(regular_ui.cancel, SIGNAL(clicked()), this, SLOT(on_actionClose_triggered()));
    connect(regular_ui.save, SIGNAL(clicked()), this, SLOT(saveRegular()));
    connect(regular_ui.remove, SIGNAL(clicked()), this, SLOT(deleteRegular()));
    
}

void regularEdit::saveRegular() {
    //��ȡYaml
    YAML::Node regular = YAML::LoadFile("data/regular.yml");

    if (mNewRegular == false) {
        //ת���ɴ洢��Regular
        string fmRegular = Helper::QString2stdString(mRegular.regular);
        string fmAction = Helper::QString2stdString(mRegular.action);
        string fmFrom;
        bool fmPermission = mRegular.permission;
        //qDebug() << "fmRegular:" << fmRegular << "\nfmAction:" << fmAction << "\nfmFrom:" << fmFrom << "\nfmPer"

        //ת����Դ
        switch (mRegular.from)
        {
        case group:
            fmFrom = "group";
            break;
        case console:
            fmFrom = "console";
            break;
        }

        //��ȡԭ����
        bool removed = false;
        for (int i = 0; i < regular.size(); i++) {
            if (regular[i]["Regular"].as<string>() == fmRegular && \
                regular[i]["Action"].as<string>() == fmAction && \
                regular[i]["From"].as<string>() == fmFrom && \
                regular[i]["Permissions"].as<bool>() == fmPermission) {
                regular.remove(i);
                removed = true;
                break;
            }
        }

        if (!removed) {
            QMessageBox::warning(
                this,
                u8"����",
                u8"�޷�����ɾ��ԭ�������ֶ�ɾ��",
                QMessageBox::Yes
            );
        }
    }
    
    //д���¶���
    //ת���ɴ洢��Regular
    string tmRegular = Helper::QString2stdString(regular_ui.regularEdit->text());
    string tmAction = Helper::QString2stdString(regular_ui.actionEdit->text());
    string tmFrom;
    bool tmPermission = regular_ui.checkBox->checkState();

    //ת����Դ
    switch (groupButton1->checkedId())
    {
    case 0:
        tmFrom = "console";
        break;
    case 1:
        tmFrom = "group";
        break;
    }
    YAML::Node newRegular;
    newRegular["Regular"] = tmRegular;
    newRegular["Action"] = tmAction;
    newRegular["From"] = tmFrom;
    newRegular["Permissions"] = tmPermission;
    regular.push_back(newRegular);

    //д���ļ�
    ofstream fout("data/regular.yml");
    fout << regular;
    fout.close();

    this->close();

    

}

void regularEdit::deleteRegular() {
    //��ȡYaml
    YAML::Node regular = YAML::LoadFile("data/regular.yml");
    //ת���ɴ洢��Regular
    string fmRegular = Helper::QString2stdString(mRegular.regular);
    string fmAction = Helper::QString2stdString(mRegular.action);
    string fmFrom;
    bool fmPermission = mRegular.permission;

    //ת����Դ
    switch (mRegular.from)
    {
    case group:
        fmFrom = "group";
        break;
    case console:
        fmFrom = "console";
        break;
    }

    //��ȡԭ����
    bool removed = false;
    for (int i = 0; i < regular.size(); i++) {
        if (regular[i]["Regular"].as<string>() == fmRegular && \
            regular[i]["Action"].as<string>() == fmAction && \
            regular[i]["From"].as<string>() == fmFrom && \
            regular[i]["Permissions"].as<bool>() == fmPermission) {
            regular.remove(i);
            removed = true;
            break;
        }
    }

    if (!removed) {
        QMessageBox::warning(
            this,
            u8"����",
            u8"�޷�����ɾ���������ֶ�ɾ��",
            QMessageBox::Yes
        );
    }

    //д���ļ�
    ofstream fout("data/regular.yml");
    fout << regular;
    fout.close();

    this->close();
}

void regularEdit::setRegular() {
    //��������
    regular_ui.regularEdit->setText(mRegular.regular);
    regular_ui.actionEdit->setText(mRegular.action);
    switch (mRegular.from)
    {
    case console:
        regular_ui.console->setChecked(true);
        break;
    case group:
        regular_ui.group->setChecked(true);
        break;
    }
    //�����Ƿ���ҪȨ��
    regular_ui.checkBox->setChecked(mRegular.permission);
    
}

void regularEdit::stratProgress() {
    QProcess process(this);
    process.startDetached("regularTester.exe");
}

///////////////////////////////////////////// Style /////////////////////////////////////////////
void regularEdit::setGraphics(QWidget* bt) {
    QGraphicsDropShadowEffect* shadow_effect = new QGraphicsDropShadowEffect(this);
    shadow_effect->setOffset(0, 0);
    shadow_effect->setColor(Qt::gray);
    shadow_effect->setBlurRadius(8);
    bt->setGraphicsEffect(shadow_effect);
}

///////////////////////////////////////////// MoveWindow /////////////////////////////////////////////
//�������ĳ���
const static int pos_min_x = 0;
const static int pos_max_x = 500;
const static int pos_min_y = 0;
const static int pos_max_y = 30;
//�Լ�ʵ�ֵĴ����϶�����
void regularEdit::mousePressEvent(QMouseEvent* event)
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
void regularEdit::mouseMoveEvent(QMouseEvent* event)
{
    if (isMousePressed == true)
    {
        QPoint movePot = event->globalPos() - mousePosition;
        move(movePot);
    }
}
void regularEdit::mouseReleaseEvent(QMouseEvent* event)
{
    isMousePressed = false;
}

void regularEdit::on_actionClose_triggered()
{
    close();
}