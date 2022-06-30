//����ͷ�ļ�
#include "websocketClient.h"
#include "global.h"
//#include "CPython.h"
#include "logger.h"
#include <qmessagebox.h>
#include <qdebug.h>
#include <string>
#include "helper.h"
#include "server.h"

using namespace std;

//��Ϣ����
unordered_map<string, string> messageType = {
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

string getConfig(string key);
//����Client
Logger mirai_logger("Mirai");

//������ṹ��
enum regularAction { Console, Group, Command };
enum regularFrom { group, console };
struct Regular {
	QString regular;
	string action;
	regularAction type;
	regularFrom from;
	bool permission;
};

//API
bool connectMirai() {
	bool connected = mirai->login();
	if (connected) {
		mirai_logger.info(u8"���ӵ�Mirai�ɹ�");
	}
	else {
		QMessageBox::critical(window, u8"Mirai����", u8"�޷����ӵ�Mirai", QMessageBox::Ok);
		mirai_logger.error(u8"�޷����ӵ� Mirai");
	}

	mirai->botProfile();
	return connected;
}

//WebSocketClinet::WebSocketClinet(QObject* parent)
//	: QObject(parent){
//	m_pWebSocket = new QWebSocket();
//	connect(
//		m_pWebSocket, SIGNAL(connected()), 
//		this, SLOT(slotConnected())
//	);
//	connect(
//		m_pWebSocket, SIGNAL(disconnected()),
//		this, SLOT(slotDisconnected())
//	);
//	connect(
//		m_pWebSocket, SIGNAL(error(QAbstractSocket::SocketError)),
//		this, SLOT(slotError(QAbstractSocket::SocketError))
//	);
//}
//
//WebSocketClinet::~WebSocketClinet(){
//	if (m_pWebSocket != 0){
//		m_pWebSocket->deleteLater();
//		m_pWebSocket = 0;
//	}
//}
//// ����websocket��������URL
//bool WebSocketClinet::connectUrl(QString url){
//	m_url = QUrl(url);
//	m_pWebSocket->open(m_url);
//	return true;
//}
//// �ر�websocket
//void WebSocketClinet::close(){
//	m_pWebSocket->close();
//}
//// ����Text���͵���Ϣ
//void WebSocketClinet::sendTextMsg(const QString& message){
//	if (!m_bConnected){
//		//mirai_logger.error("{} {} Failed to {}, it's not running...", __FILE__ , __LINE__,__FUNCTION__);
//		return;
//	}
//	//mirai_logger.debug("Send: {}", message);
//	m_pWebSocket->sendTextMessage(message);
//}
//// ����Text���͵���Ϣ
//void WebSocketClinet::sendTextMsg(const string& message){
//	QString m_message = Helper::stdString2QString(message);
//	if (!m_bConnected){
//		//mirai_logger.error("{} {} Failed to {}, it's not running...", __FILE__, __LINE__, __FUNCTION__);
//		return;
//	}
//	//mirai_logger.debug("Send: {}", message);
//	m_pWebSocket->sendTextMessage(m_message);
//}
//// ����Binary���͵���Ϣ
//void WebSocketClinet::sendBinaryMsg(const QByteArray& data){
//	if (!m_bConnected){
//		//mirai_logger.error("{} {} Failed to {}, it's not running...", __FILE__, __LINE__, __FUNCTION__);
//		return;
//	}
//	m_pWebSocket->sendBinaryMessage(data);
//}
//// ���ط���������״̬
//bool WebSocketClinet::getConStatus(){
//	return m_bConnected;
//}
//// ���ӳɹ�
//void WebSocketClinet::slotConnected(){
//	mirai_logger.info(u8"Websocket���ӳɹ�.");
//	m_bConnected = true;
//	connect(m_pWebSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(slotRecvTextMsg(QString)));
//	connect(m_pWebSocket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(slotRecvBinaryMsg(QByteArray)));
//}
//// �Ͽ�����
//void WebSocketClinet::slotDisconnected(){
//	mirai_logger.error(u8"Websocket�ѶϿ�����"); 
//	reconnect();
//}
//// �����ַ�����
//void WebSocketClinet::slotRecvTextMsg(QString message){
//	emit sigRecvTextMsg(message);
//}
//// ���ܶ���������
//void WebSocketClinet::slotRecvBinaryMsg(QByteArray message){
//	mirai_logger.debug("slotRecvBinaryMsg: {}", message);
//}
//// ��Ӧ����
//void WebSocketClinet::slotError(QAbstractSocket::SocketError error){
//	mirai_logger.error("errorCode:{} error:{}", (int)error, Helper::QString2stdString(m_pWebSocket->errorString()));
//}
//// �Ͽ�����
//void WebSocketClinet::reconnect() { 
//	mirai_logger.warn(u8"Websocket������..."); 
//	m_pWebSocket->abort(); 
//	m_pWebSocket->open(m_url);
//}

WsClient::WsClient() {
	ws.OnTextReceived([]
	(WebSocketClient& client, string msg) {mirai->onText(client, msg);});
	ws.OnError([]
	(WebSocketClient& client, string msg) {mirai->onError(client, msg);});
	ws.OnLostConnection([]
	(WebSocketClient& client, int code) {mirai->onLost(client, code);});
}

int WsClient::getStatus() {
	return (int)ws.GetStatus();
}

bool WsClient::connect(string url) {
	try {
		ws.Connect(url);
		this->connected = true;
		return true;
	}
	catch (std::runtime_error e) {
		return false;
	}
}

bool WsClient::close() {
	if (this->connected) {
		try {
			ws.Close();
			return true;
		}
		catch (std::runtime_error e) {}
	}
	return false;
}

bool WsClient::sendTextMsg(string msg) {
	if (this->connected) {
		try {
			ws.SendText(msg);
			return true;
		}
		catch (std::runtime_error e) {}
	}
	return false;
}

bool WsClient::shutdown() {
	if (this->connected) {
		try {
			ws.Shutdown();
			return true;
		}
		catch (std::runtime_error e) {}
	}
	return false;
}

Mirai::Mirai() {
	wsc = new WsClient();
}

void Mirai::selfGroupCatchLine(Message message) {
	YAML::Node regular = YAML::LoadFile("data/regular.yml");
	YAML::Node config = YAML::LoadFile("config/config.yml");

	vector<Regular> regularList;

	//��ȡ������
	for (YAML::Node i : regular) {
		string mRegular = i["Regular"].as<string>();
		string Action = i["Action"].as<string>();
		string From = i["From"].as<string>();
		bool Permissions = i["Permissions"].as<bool>();

		//ת��type
		string Action_type = Action.substr(0, 2);
		regularAction regular_action;
		if (Action_type == "<<") { regular_action = regularAction::Console; }
		else if (Action_type == ">>") { regular_action = regularAction::Group; }
		else { regular_action = regularAction::Command; };

		//ת����Դ
		regularFrom regular_from;
		transform(From.begin(), From.end(), From.begin(), ::toupper);
		if (From == "group") { regular_from = regularFrom::group; }
		else { regular_from = regularFrom::console; };
		Regular regular = { Helper::stdString2QString(mRegular),Action.erase(0, 2),regular_action,regular_from,Permissions };
		//����vector
		regularList.push_back(regular);
	}
	//ʹ��������
	for (Regular i : regularList) {
		QRegExp r(i.regular);
		int r_pos = r.indexIn(Helper::stdString2QString(message.message));
		//bool qqAdmin = std::find(config["admin"].begin(), config["admin"].end(), message.qqNum) != config["admin"].end();
		bool qqAdmin = true;

		//ִ�в���
		if (r_pos > -1 && i.from == regularFrom::group && (i.permission == false || (i.permission == true && qqAdmin))) {
			int num = 0;
			for (auto& replace : r.capturedTexts()) {
				i.action = Helper::replace(i.action, "$" + std::to_string(num), Helper::QString2stdString(replace));
				num++;
			}
			//ִ�в���
			string cmd = fmtConsole::FmtGroupRegular(message.qqNum, message.qqNick, i.action);
			if (i.type == regularAction::Console) {
				server->sendCmd(cmd + "\n");
			}
			else if (i.type == regularAction::Group) {
				mirai->sendAllGroupMsg(cmd);
			}
			else {
				Basic::Command::CustomCmd(cmd, message.group);
			}
		}
	}
}

//Msg
void Mirai::onText(WebSocketClient& client, string msg) {
	json msg_json = json::parse(msg);
	string syncId = msg_json["syncId"].get<string>();
	mirai_logger.debug(msg_json.dump());
	//window->PacketCallback(msg);
	//��¼��
	if (syncId == "1") {
		//����UserImage
		string qqNick = msg_json["data"]["nickname"].get<string>();
		mirai_logger.info(u8"{}��¼ Mirai �ɹ�", qqNick);
		logined = true;
		emit OtherCallback("onLogin");
		emit setUserImages(Helper::stdString2QString(getConfig("qq")), Helper::stdString2QString(qqNick));
	}
	//����Ϣ��
	else if (syncId == "2") {
		int msgId = msg_json["data"]["messageId"];
		if (msgId == -1) {
			mirai_logger.warn(u8"�ѷ�����Ϣ�������⵽����");
		}
	}
	else if (syncId == "-1") {
		//��Ϣ����
		if (msg_json["data"].find("type") != msg_json["data"].end() && msg_json["data"]["type"] == "GroupMessage") {
			string qq = std::to_string(msg_json["data"]["sender"]["id"].get<long long>());
			string qqnick = msg_json["data"]["sender"]["memberName"].get<string>();
			string group = std::to_string(msg_json["data"]["sender"]["group"]["id"].get<long long>());
			string msg = "";

			vector<string> allowGroup;
			std::ifstream fin("config/config.yml");
			YAML::Node config = YAML::Load(fin);
			for (auto i : config["group"]) {
				allowGroup.push_back(i.as<string>());
			}

			if (std::find(allowGroup.begin(), allowGroup.end(), group) != allowGroup.end()) {
				for (auto& i : msg_json["data"]["messageChain"]) {
					//������Ϣ
					if (i["type"] == "Plain") {
						msg += u8" " + i["text"].get<string>();
					}
					else if (i["type"] == "At") {
						msg += u8" " + i["display"].get<string>();
					}
					else if (i["type"] == "File") {
						msg += u8" [�ļ�]" + i["name"].get<string>();
					}
					else if (i["type"] == "MusicShare") {
						msg += u8" [���ַ���]" + i["musicUrl"];
					}
					else if (i["type"] == "Source") {}
					else {
						if (std::find(messageType.begin(), messageType.end(), i["type"]) != messageType.end()) {
							msg += messageType[i["type"]];
						}
						else {
							msg += u8" [δ֪��Ϣ]";
						}
					}
				}
				msg = msg.erase(0, 1);
				//�洢�ṹ��
				Message message = { group,qq,msg,qqnick };

				//Callback
				std::unordered_map<string, string> args;
				args.emplace("group", group);
				args.emplace("msg", msg);
				args.emplace("qq", qq);
				args.emplace("qqnick", qqnick);
				emit OtherCallback("onReceiveMsg", args);
				selfGroupCatchLine(message);

			}
		}
		//�¼�����(Ⱥ��Ա����)
		else if (msg_json["data"].find("type") != msg_json["data"].end() && \
			msg_json["data"]["type"] == "MemberCardChangeEvent") {

		}
	}

}

//��Ӧ���ӱ���
void Mirai::onError(WebSocketClient& client, string msg) {
	mirai_logger.error("WebsocketClient error:{}", msg);
	emit OtherCallback("onConnectError");
}

//��Ӧ���Ӷ�ʧ
void Mirai::onLost(WebSocketClient& client, int code) {
	mirai_logger.error("WebsocketClient Connect Lost,errorCode:{}", code);
	emit OtherCallback("onConnectLost");
}

bool Mirai::login() {
	try {
		string qq = getConfig("qq");
		string key = getConfig("key");
		string wsUrl = getConfig("connectUrl");

		//ƴ��URL
		string url = wsUrl + "/all?verifyKey=" + key + "&qq=" + qq;
		return wsc->connect(url);
	}
	catch (const char e) {
		return false;
	}
}

void Mirai::botProfile() {
	string mj = "{\"syncId\": 1,\"command\" : \"botProfile\",\"subCommand\":null,\"content\":{}}";
	wsc->sendTextMsg(mj);
}

void Mirai::sendGroupMsg(string group, string msg, bool callback) {
	if (this->logined) {
		string mj = "{\"syncId\": 2, \"command\":\"sendGroupMessage\", \"subCommand\" : null,\
					\"content\": {\"target\":" + group + ", \"messageChain\": [{ \"type\":\"Plain\", \"text\" : \"" + msg + "\"}]}}";
		std::unordered_map<string, string> args;
		args.emplace("group", group);
		args.emplace("msg", msg);
		emit OtherCallback("onSendMsg", args);
		wsc->sendTextMsg(mj);
	}

}

void Mirai::recallMsg(string target, bool callback) {
	//recall
	if (logined) {
		string mj = "{\"syncId\": 3,\"command\" : \"recall\",\"subCommand\":null,\"content\":{\"target\":" + target + "}}";
		std::unordered_map<string, string> args;
		args.emplace("target", target);
		emit OtherCallback("onRecall", args);
		wsc->sendTextMsg(mj);
	}
}

void Mirai::sendAllGroupMsg(string msg, bool callback) {
	YAML::Node config = YAML::LoadFile("config/config.yml")["group"];
	for (auto igroup : config) {
		string group = std::to_string(igroup.as<long long>());
		if (logined) {
			sendGroupMsg(group, msg, callback);
		}
	}

}

void Mirai::changeName(string qq, string group, string name) {
	string packet_Json = "{\"syncId\": 4,\"command\" : \"memberInfo\",\"subCommand\":\"update\",\"content\":{\"target\":" + group + ",\"memberId\":" + qq + ",\"info\" : {\"name\":" + name + "}}}";
	if (logined) {
		wsc->sendTextMsg(packet_Json);
	}
}

void Mirai::send_app(string group, string code) {
	string packet_Json = "{\"syncId\": 5,\"command\" : \"sendGroupMessage\",\"subCommand\" : null,\"content\" : {\"target\":" + group + ",\"messageChain\" : [{\"type\": \"App\",\"content\" :" + code + "}]}}";
	if (logined) {
		wsc->sendTextMsg(packet_Json);
	}
}

void Mirai::SendPacket(string packet) {
	wsc->sendTextMsg(packet);
}