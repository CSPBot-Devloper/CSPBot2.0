#pragma once

#include "helper.h"
#include <QObject>
#include <QtWebSockets>
#include <QDebug>
#include <QUrl>
#include <LightWSClient/WebSocketClient.h>
#include "cspbot20.h"
#include <magic_enum.hpp>

using namespace cyanray;

class WsClient :public QObject
{
	Q_OBJECT
private:
	WebSocketClient ws;
	bool connected = false;
signals:
	void sigConnected(mTime nowTime); //���ӳɹ�
	void setUserImages(QString qqNum, QString qqNick); //����UserImages
	void updateSendRecive(int send,int recive); //���½���
public:
	explicit WsClient();
	//API
	bool sendTextMsg(string msg);
	bool connect(string url);
	bool close();
	bool shutdown();
	int getStatus();
};



std::string getConfig(std::string key);

typedef std::string groupNum;
typedef std::string qqNum;

enum Permission {
	OWNER,
	ADMINISTRATOR,
	MEMBER
};

struct messagePacket {
	std::string message;
	groupNum group;
	std::string groupName;
	qqNum qq;
	std::string memberName;
	std::string syncId;
	Permission perm;
};

//��Ϣ����
inline std::unordered_map<std::string, std::string> messageType = {
	{"Image",u8" [ͼƬ]"},
	{"Quote",u8" [�ظ�����Ϣ]"},
	{"AtAll",u8" [@ȫ���Ա]"},
	{"Face",u8" [QQ����]"},
	{"FlashImage",u8" [����]"},
	{"Voice",u8" [����]"},
	{"Xml",u8" [XML��Ϣ]"},
	{"Json",u8" [JSON��Ϣ]"},
	{"App",u8" [С����]"},
	{"Poke",u8" [��һ��]"},
	{"Dice",u8" [δ֪��Ϣ]"},
	{"ForwardMessage",u8" [ת������Ϣ]"}
};

inline std::string transMessage(json j) {
	std::string msg = "";
	for (auto& i : j["data"]["messageChain"]) {
		//������Ϣ
		if (i["type"] == "Plain") { msg += u8" " + i["text"].get<std::string>(); }
		else if (i["type"] == "At") { msg += u8" " + i["display"].get<std::string>(); }
		else if (i["type"] == "File") { msg += u8" [�ļ�]" + i["name"].get<std::string>(); }
		else if (i["type"] == "MusicShare") { msg += u8" [���ַ���]" + i["musicUrl"]; }
		else if (i["type"] == "Source") {}
		else {
			if (std::find(messageType.begin(), messageType.end(), i["type"]) != messageType.end()) {
				msg += messageType[i["type"]];
			}
			else { msg += u8" [δ֪��Ϣ]"; }
		}
	}
	return msg;
}

inline messagePacket transMessagePacket(json j) {
	qqNum qq = std::to_string(j["data"]["sender"]["id"].get<long long>());
	std::string qqnick = j["data"]["sender"]["memberName"].get<std::string>();
	groupNum group = std::to_string(j["data"]["sender"]["group"]["id"].get<long long>());
	std::string message = transMessage(j);
	std::string syncId = j["syncId"].get<string>();
	std::string groupName = j["data"]["sender"]["group"]["name"].get<std::string>();
	std::string permissionString = j["data"]["sender"]["permission"].get<std::string>();
	auto permission = magic_enum::enum_cast<Permission>(permissionString);
	messagePacket msgPacket = {
		message,
		group,
		groupName,
		qq,
		qqnick,
		syncId,
		permission.value()
	};
	return msgPacket;
}


class Mirai :public QObject
{
	Q_OBJECT
public:
	explicit Mirai();
	bool login();
	void sendGroupMsg(std::string group, std::string msg, bool callback = true);
	void sendAllGroupMsg(std::string msg, bool callback = true);
	void recallMsg(std::string target, bool callback = true);
	void botProfile();
	void changeName(std::string qq, std::string group, std::string name);
	void send_app(std::string group, std::string code);
	void SendPacket(std::string packet);
	bool logined = false;

	//Callback Event
	void onText(WebSocketClient& client, string msg);
	void onError(WebSocketClient& client, string msg);
	void onLost(WebSocketClient& client, int code);
signals:
	void setUserImages(QString qqNum, QString qqNick); //����UserImages
	void OtherCallback(QString listener, StringMap args={}); //Callback
	void sigLogin(); //��¼�ɹ�
	void updateSendRecive(int send, int recive); //���½���
	void signalConnect(mTime time); //���ӳɹ�����
private slots:
	void slotUpdateSendRecive(int send, int recive);
	void slotConnected(mTime time); //���ӳɹ�
	void slotSetUserImages(QString qqNum, QString qqNick);//�����û�ͼƬ
private:
	void selfGroupCatchLine(messagePacket message);
	//WsClient* wsc;
};

class CommandAPI :public QObject
{
	Q_OBJECT
signals:
	void signalStartServer();
	//void Callback(QString listener,StringMap args);
public:
	explicit CommandAPI(){};
	std::vector<std::string> SplitCommand(const std::string& parent);
	void CustomCmd(std::string cmd, std::string group);
};