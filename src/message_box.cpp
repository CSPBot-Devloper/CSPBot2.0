#include "global.h"
#include <qstring.h>
#include "cspbot20.h"
#include "message_box.h"

//��ʾError��Ϣ
void msgbox::ShowError(const QString& msg) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Icon::Critical);
	msgBox.setText(msg);
	msgBox.exec();
}

//��ʾInformation��Ϣ
void msgbox::ShowHint(const QString& msg) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Icon::Information);
	msgBox.setText(msg);
	msgBox.exec();
	
}

//��ʾWarning��Ϣ
void msgbox::ShowWarn(const QString& msg) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Icon::Warning);
	msgBox.setText(msg);
	msgBox.exec();
}

//��ʾQuestion��Ϣ
void msgbox::ShowQuestion(const QString& msg) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Icon::Question);
	msgBox.setText(msg);
	msgBox.exec();
}
