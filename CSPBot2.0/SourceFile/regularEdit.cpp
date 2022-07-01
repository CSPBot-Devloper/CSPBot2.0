#include "regularEdit.h"
#include <QGraphicsDropShadowEffect>
#include <qevent.h>
#include "qprocess.h"

using namespace std;

regularEdit::regularEdit(Regular regular,QWidget* parent)
    : QWidget(parent)
{
    regular_ui.setupUi(this);
    mRegular = regular;

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
    };
    for (QWidget* i : widget) {
        setGraphics(i);
    }

    connect(regular_ui.close, SIGNAL(clicked()), this, SLOT(on_actionClose_triggered()));
    //����
    groupButton1 = new QButtonGroup(this);
    groupButton1->addButton(regular_ui.console, 0);
    groupButton1->addButton(regular_ui.group, 1);
    //ui->apple_radioButton->setChecked(true);

    groupButton2 = new QButtonGroup(this);
    groupButton2->addButton(regular_ui.runConsole, 0);
    groupButton2->addButton(regular_ui.runCommand, 1);
    groupButton2->addButton(regular_ui.runGroup, 2);

    setRegular();

    ///////// Bind //////////
    connect(regular_ui.regularTest, SIGNAL(clicked()), this, SLOT(stratProgress()));
}

void regularEdit::setRegular() {
    //��������
    regular_ui.regularEdit->setText(mRegular.regular);
    regular_ui.actionEdit->setText(Helper::stdString2QString(mRegular.action));
    //���õ�ѡ��
    switch (mRegular.type) {
        case Console:
            regular_ui.runConsole->setChecked(true);
            break;
        case Group:
            regular_ui.runGroup->setChecked(true);
            break;
        case Command:
            regular_ui.runCommand->setChecked(true);
            break;
    }
    switch (mRegular.from)
    {
    case console:
        regular_ui.console->setCheckable(true);
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