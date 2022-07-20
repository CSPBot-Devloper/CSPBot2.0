#include "global.h"
#include <qstring.h>
#include "cspbot20.h"
#include "dialog.h"
#include "message_box.h"

void msgbox::ShowError(const QString& msg) {
	//CDialog* s_dialogWindow = new CDialog(diaLogStatus::error, g_main_window);
	//s_dialogWindow->setTitle("���ش���");
	//s_dialogWindow->setContent(msg);
	//s_dialogWindow->exec();
	QMessageBox msgBox;
	msgBox.setText(msg);
	msgBox.exec();
	//QMessageBox::critical(g_main_window, "���ش���", msg, QMessageBox::Yes);
}

void msgbox::ShowHint(const QString& msg) {
	//CDialog* s_dialogWindow = new CDialog(diaLogStatus::info, g_main_window);
	//s_dialogWindow->setTitle("��ʾ");
	//s_dialogWindow->setContent(msg);
	//s_dialogWindow->exec();
	QMessageBox msgBox;
	msgBox.setText(msg);
	msgBox.exec();
	//QMessageBox::information(g_main_window, "��ʾ", msg, QMessageBox::Yes);
}

void msgbox::ShowWarn(const QString& msg) {
	//CDialog* s_dialogWindow = new CDialog(diaLogStatus::warn, g_main_window);
	//s_dialogWindow->setTitle("����");
	//s_dialogWindow->setContent(msg);
	//s_dialogWindow->exec();
	QMessageBox msgBox;
	msgBox.setText(msg);
	msgBox.exec();
	//QMessageBox::warning(g_main_window, "����", msg, QMessageBox::Yes);
}
