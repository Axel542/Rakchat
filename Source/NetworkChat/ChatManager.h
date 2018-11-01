//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//Created by Angus Secomb           |
//Last modified: 23/05/18			|
//Header for NetworkChat Class      |
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#include <iostream>
#include <string>

#include <RakPeerInterface.h>
#include <BitStream.h>
#include <thread>
#include <chrono>
#include <MessageIdentifiers.h>
#include <fstream>
#include <iostream>
#include "WindowsIncludes.h"
#include <list>
#include <vector>

enum NetMessages
{
	ID_SERVER_TEXT_MESSAGE = ID_USER_PACKET_ENUM + 1,
	ID_SERVER_SET_CLIENT_ID,
	ID_CLIENT_DATA,
	ID_MESSAGE_TEXT,
	ID_USER_DATA,
	ID_SERVER_SHUTDOWN,
	ID_LOGIN_PROCEDURE,
	ID_USER_KICKED,
	ID_USER_WHISPER,
};

struct UserClient
{
	std::string userName;
	std::string password;
	bool loginSuccessful = false;
};

class ChatManager
{
public:
	ChatManager();
	~ChatManager();

	void handleServerNetworkMessages(RakNet::RakPeerInterface* a_peerInterface);

	void handleClientNetworkMessages(RakNet::RakPeerInterface* a_peerInterface);

	void startClientConnection(const char* IP, const unsigned short port);

	void onSetClientIDPacket(RakNet::Packet* packet);

	void receiveInputData();

	void sendUserData();

	void receiveLoginData(RakNet::Packet* packet);

	void printUserText(RakNet::Packet* packet);
	
	//An example of how to implement user accounts
	//to chat system through console.
	void consoleClientLoginCheck();

	char* getInputData(char* str, int num, bool check);

	//Start server instance.
	void createServer(int maxConnections, const char* ip, const unsigned short port);

	void sentNewClientID(RakNet::RakPeerInterface* a_peerInterface,
		RakNet::SystemAddress& address);

	void handleServerCommands(int a_int, std::string command);

	RakNet::RakPeerInterface* getRPInterface() { return rpInterface; }

protected:

	//checks if the file currently exists.
	bool fileExists(const std::string& filename);

	//checks if an element is in a string array.
	bool inArray(const std::string &value, const std::vector<std::string> &array);

	char* m_IP;
	std::string sIP;
	short m_PORT = 5456;
	RakNet::RakPeerInterface* rpInterface;

	int m_clientID;
	int nextClientID;
	bool isClientConnected = false;

	bool isKicked = false;
	
	UserClient userDetails;

	std::vector<std::string> commandList = { "!help", "!whisper", "!kick", "!online",
											 "!shutdown", "!say", "!exit", "!logout",
											 "!connect"};
	
	std::vector<std::string> connectedUserList;
};

