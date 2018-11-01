//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//Created by Angus Secomb           |
//Last modified: 23/05/18			|
//cpp for NetworkChat Class         |
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#include "ChatManager.h"

ChatManager::ChatManager()
{
	rpInterface = RakNet::RakPeerInterface::GetInstance();
	nextClientID = 1;
	m_clientID = 0;
}

ChatManager::~ChatManager()
{
}

//Gives client new id when connecting.
void ChatManager::sentNewClientID(RakNet::RakPeerInterface * a_peerInterface, RakNet::SystemAddress & address)
{
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)ID_SERVER_SET_CLIENT_ID);
	bs.Write(nextClientID);
	nextClientID++;

	a_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED,
		0, address, false);
}

//Handles server input commands.
void ChatManager::handleServerCommands(int a_int, std::string command)
{
	bool check = false;

	while (true)
	{
		std::cin >> command;
		switch (a_int)
		{
			//Server
		case 0:
			if (inArray(command, commandList))
			{
				for (int i = 0; i < commandList.size(); ++i)
				{
					if (commandList[i] == command)
					{
						switch (i)
						{
						case 0:
							std::cout << "Commands:" << std::endl;
							std::cout << "!help - the help function lists the functions below in console." << std::endl;
							std::cout << "!kick[userName] - Kicks user from the server" << std::endl;
							std::cout << "!online - gets users online." << std::endl;
							std::cout << "!shutdown - Closes server and application." << std::endl;
							std::cout << "!say[message] - Sends global server message." << std::endl;
							break;
						case 2:
						{
							RakNet::BitStream bs;
							bs.Write((RakNet::MessageID)ID_USER_KICKED);

							std::string user;
							//std::cout << "Kick user: ";
							std::cin >> user;

						//	bs.Write(user);
							
							for (int i = 0; i < connectedUserList.size(); ++i)
							{
								if (user == connectedUserList[i])
								{
									connectedUserList[i].erase();
									std::cout << user << " kicked." << std::endl;
									rpInterface->CloseConnection(rpInterface->GetGUIDFromIndex(i), true,
										RELIABLE_ORDERED);
								}
							}
							int num;
							num = connectedUserList.size();
							bs.Write(num);
							RakNet::RakString USERS;
							RakNet::BitStream bsOUT;

							
							for (int i = 0; i < num; ++i)
							{
								
								bsOUT.Write(RakNet::RakString(connectedUserList[i].c_str()));
							//	connectedUserList.push_back(users.C_String());
							}

							rpInterface->Send(&bsOUT, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
							rpInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
							break;
						}
						case 3:
							std::cout << "Online: ";

							for (int i = 0; i < connectedUserList.size(); ++i)
							{
								if (connectedUserList[i] != "")
								{
									if (i == (connectedUserList.size() - 1))
									{
										std::cout << connectedUserList[i] << std::endl;
									}
									else
									{
										std::cout << connectedUserList[i] << ", ";
									}
								}
							}
							break;
						case 4:
						{
							RakNet::BitStream bs;
							bs.Write((RakNet::MessageID)ID_SERVER_SHUTDOWN);
							rpInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
							rpInterface->Shutdown(0);

							std::cout << "Server is shutting down.." << std::endl;
							std::cout << "Server is shut. Application is now safe to close." << std::endl;							
							break;
						}
						case 5:
							RakNet::RakString messageRS;
							char info[2048];
							getInputData(info, sizeof(info), true);
							std::cout << "Server: " << info << std::endl;

							messageRS = info;
							RakNet::BitStream BS;
							BS.Write((RakNet::MessageID)ID_SERVER_TEXT_MESSAGE);
							BS.Write(messageRS);

							rpInterface->Send(&BS, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
							break;
						}
					}
				}
			}
		}

		command = "";
	}
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

//Handles the packets server side.
void ChatManager::handleServerNetworkMessages(RakNet::RakPeerInterface* a_peerInterface)
{
	//Create a packet.
	RakNet::Packet* packet = nullptr;

	while (true)
	{
		//for each packet received.
		for (packet = a_peerInterface->Receive(); packet;
			a_peerInterface->DeallocatePacket(packet),
			packet = a_peerInterface->Receive())
		{
			//Check net message and do stuff.
			switch (packet->data[0])
			{
			case ID_NEW_INCOMING_CONNECTION:
				//set new client id.
				std::cout << "Incoming connection... \n";
				sentNewClientID(a_peerInterface, packet->systemAddress);
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				std::cout << "A client has disconnected. \n";
				break;
			case ID_CONNECTION_LOST:
				std::cout << "A client has lost the connection. \n";
				break;
			case ID_MESSAGE_TEXT:
			{
				//Handles user messages.
				//Grabs packets and prints.
				RakNet::RakString rs;
				RakNet::BitStream bs(packet->data, packet->length, false);
				a_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
									 packet->systemAddress, true);
				bs.IgnoreBytes(sizeof(RakNet::MessageID));
				printUserText(packet);			

				break;
			}
			case ID_USER_DATA:
			{
				//Gets login data from client.
				receiveLoginData(packet);
				break;
			}
			case ID_SERVER_SHUTDOWN:
				break;
			case ID_LOGIN_PROCEDURE:
				break;
			case ID_SERVER_TEXT_MESSAGE:
				break;
			case ID_USER_KICKED:
				break;
			case ID_USER_WHISPER:
			{
				//Receive the whisper data
				//from the user.
				RakNet::RakString user;
				RakNet::RakString message;
				RakNet::RakString wUser;
				RakNet::BitStream bs(packet->data, packet->length, false);
				a_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
					packet->systemAddress, true);
				bs.IgnoreBytes(sizeof(RakNet::MessageID));
				bs.Read(user);
				bs.Read(wUser);
				bs.Read(message);
			
				//Send out the whisper data again.
				RakNet::BitStream BSOUT;
				BSOUT.Write(user);
				BSOUT.Write(message);

				a_peerInterface->Send(&BSOUT, HIGH_PRIORITY, RELIABLE, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
				
				//Let server know who whispered to who.
				std::cout << wUser.C_String() << " whispered """ << message.C_String() << """ to " << user.C_String() << std::endl;
				break;
			}
				
			default:
				std::cout << "Receive a message with a unknown id: " << packet->data[0];
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	
}

//Function bad
//need to fix server messages data is getting extremely mixed up.
void ChatManager::handleClientNetworkMessages(RakNet::RakPeerInterface * a_peerInterface)
{
	RakNet::Packet* packet = nullptr;
	while (true)
	{
		for (packet = a_peerInterface->Receive(); packet;
			a_peerInterface->DeallocatePacket(packet),
			packet = a_peerInterface->Receive())
		{
			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				std::cout << "Another client has disconnected.\n";
				break;
			case ID_REMOTE_CONNECTION_LOST:
				std::cout << "Another client has lost connection.\n";
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				std::cout << "Another client has connected.\n";
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				std::cout << "Our connection request has been accepted.\n";
				break;
			}
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				std::cout << "Server is full.\n";
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				std::cout << "You have been kicked.\n";
				isKicked = true;
				break;
			case ID_CONNECTION_LOST:
				std::cout << "Connection lost.\n";
				break;
			case ID_SERVER_SET_CLIENT_ID:
				onSetClientIDPacket(packet);
				break;
			case ID_MESSAGE_TEXT:
			{
				printUserText(packet);
				break;
			}
			case ID_USER_DATA:
			{
				sendUserData();
				break;
			}
			case ID_SERVER_SHUTDOWN:
				std::cout << "Server has shutdown" << std::endl;
				break;
			case ID_LOGIN_PROCEDURE:
			{
				//Check that login was successful,
				//update the user list,
				//if login was not successful repeat procedure.
				RakNet::RakString users;
				int usercount;
				RakNet::BitStream bs(packet->data, packet->length, false);
				a_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);
				bs.IgnoreBytes(sizeof(RakNet::MessageID));
				bs.Read(userDetails.loginSuccessful);
				bs.Read(usercount);
				for (int i = 0; i < usercount; ++i)
				{
					
					bs.Read(users);
					connectedUserList.push_back(users.C_String());
				}

				if (!userDetails.loginSuccessful)
				{
					std::cout << "Login failed: Incorrect details, \n User has already connected \n or Account already exists." << std::endl;
					consoleClientLoginCheck();
					sendUserData();
				}
				else
				{
					std::cout << "Login succesful" << std::endl;
				}

			}
			case ID_SERVER_TEXT_MESSAGE:
			{
				//Get server message and print it.
				RakNet::RakString messageRS;
				RakNet::BitStream bs(packet->data, packet->length, false);
				bs.IgnoreBytes(sizeof(RakNet::MessageID));
				bs.Read(messageRS);

				if (messageRS.C_String() != "")
				{
					std::cout << "Server: " << messageRS.C_String() << std::endl;
				}
				break;
			}
			case ID_USER_KICKED:
			{
				//Get users, clear the list and push new list contents.
				int usercount;
				RakNet::RakString users;
				std::string string;
				RakNet::BitStream bs(packet->data, packet->length, false);
				bs.IgnoreBytes(sizeof(RakNet::MessageID));
				bs.Read(usercount);
				connectedUserList.clear();

				for (int i = 0; i < usercount; ++i)
				{
					bs.Read(users);
					connectedUserList.push_back(users.C_String());
				}
				break;
			}
			case ID_USER_WHISPER:
			{
				//Get message, check to see if the clients username matches
				//the target whisper user.
				RakNet::BitStream bs(packet->data, packet->length, false);
				bs.IgnoreBytes(sizeof(RakNet::MessageID));
				RakNet::RakString username;
				RakNet::RakString message;
				RakNet::RakString wUser;
				bs.Read(username);
				bs.Read(wUser);
				bs.Read(message);

				if ((username.C_String() == userDetails.userName) && (username.C_String() != wUser.C_String()))
				{
					std::cout << "(W) " << wUser.C_String() << ": " << message.C_String() << std::endl;
				}

				break;
			}
			default:
				//If none of the messages are in the switch.
				std::cout << "Recieved a message with a unknown id: " <<
					packet->data[0];
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	std::this_thread::sleep_for(std::chrono::seconds(3));
}

//starts clients connection to server.
void ChatManager::startClientConnection(const char* IP, const unsigned short port)
{
	while (!isClientConnected)
	{

		sIP = IP;
		m_PORT = port;

		//Create a socket descriptor to describe this connection
		//No data needed, as we will be connecting to a server
		RakNet::SocketDescriptor sd;

		//now call startup - max of 1 connections (to the server)
		rpInterface->Startup(1, &sd, 1);

		std::cout << "Connecting to server at : " << IP << std::endl;

		//now Call connect to attemtp to connect to given server
		RakNet::ConnectionAttemptResult res = rpInterface->Connect(IP, port, nullptr, 0);

	
		//Final check to see if we connected and if not we throw a error
		if (res != RakNet::CONNECTION_ATTEMPT_STARTED)
		{
			std::cout << "Unable to start connection, Error number: " << res << std::endl;
		}
		else
		{
			isClientConnected = true;
		}
	}
}


//sets clients ID.
void ChatManager::onSetClientIDPacket(RakNet::Packet * packet)
{
	RakNet::BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
	bsIn.Read(m_clientID);
	std::cout << "Server has my client ID to: " << m_clientID << std::endl;
}

//(Client input function)
//Grabs clients messages and also handles client commands.
void ChatManager::receiveInputData()
{
	bool inputActive = true;
	while (inputActive)
	{
		while (userDetails.loginSuccessful)
		{
			char info[2048];
			getInputData(info, sizeof(info), true);
			//std::cin >> info;
			RakNet::BitStream BS;
			BS.Write((RakNet::MessageID)ID_MESSAGE_TEXT);
			BS.Write(RakNet::RakString(userDetails.userName.c_str()));

			//Client
			if (inArray(info, commandList))
			{
				for (int i = 0; i < commandList.size(); ++i)
				{
					if (commandList[i] == info)
					{
						switch (i)
						{
						case 0:
							std::cout << "Commands:" << std::endl;
							std::cout << "!help - the help function lists the functions below in console." << std::endl;
							std::cout << "!whisper [user] - sends a private message to a user." << std::endl;
							std::cout << "!online - shows users online." << std::endl;
							std::cout << "!exit - Closes application." << std::endl;
							std::cout << "!say[message] - Sends global server message." << std::endl;
							std::cout << "!connect - type to bring up ip and port prompt for connecting to server." << std::endl;
							break;
						case 1:
						{
							RakNet::BitStream bsW;
							std::string message;
							char user[16];

							//getInputData(user, sizeof(user), true);
							std::cin >> user;
							std::cout << "Whisper to " << user << ": ";
							std::cin >> message;
							bsW.Write((RakNet::MessageID)ID_USER_WHISPER);
							bsW.Write(RakNet::RakString(user));
							bsW.Write(RakNet::RakString(userDetails.userName.c_str()));
							bsW.Write(RakNet::RakString(message.c_str()));
							rpInterface->Send(&bsW, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
							break;
						}
						case 3:
							std::cout << "Online: ";
							for (int i = 0; i < connectedUserList.size(); ++i)
							{
								if (connectedUserList[i] != "")
								{
									if (i == (connectedUserList.size() - 1))
									{
										std::cout << connectedUserList[i] << std::endl;
									}
									else
									{
										std::cout << connectedUserList[i] << ", ";
									}
								}
							}
							break;
						case 7:
							rpInterface->CloseConnection(rpInterface->GetGUIDFromIndex(m_clientID - 1), true,
								RELIABLE_ORDERED, LOW_PRIORITY);
							break;
						case 8:
							if (!isKicked)
							{
								std::cout << "You are already connected to a server!" << std::endl;
							}
							else
							{
								char newIP[20];
								short newPort;
								std::cout << "Address: ";
								std::cin >> newIP;
								std::cout << "Port: ";
								std::cin >> newPort;
								isClientConnected = false;
								startClientConnection(newIP, newPort);
								isKicked = false;
							}
							break;
						default:
							break;
						}
					}
				}
			}
			else
			{
				BS.Write(RakNet::RakString(info));
				BS.Write(info);
			}
			rpInterface->Send(&BS, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
		}
	}
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

//Send user data from login procedure.
void ChatManager::sendUserData()
{
		RakNet::BitStream BS;
		BS.Write((RakNet::MessageID)ID_USER_DATA);
		BS.Write(RakNet::RakString(userDetails.userName.c_str()));
		BS.Write(RakNet::RakString(userDetails.password.c_str()));

		rpInterface->Send(&BS, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

//Receive login data and check again the system,
//for the existence of the account.
void ChatManager::receiveLoginData(RakNet::Packet* packet)
{
	std::cout << "Logging in..." << std::endl;
	//Bitstream 
	RakNet::BitStream bsIN(packet->data, packet->length, false);
	bsIN.IgnoreBytes(sizeof(RakNet::MessageID));

	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)ID_LOGIN_PROCEDURE);

	bool loginSuccessful = false;
	char userName[16];
	char password[16];

	//Read data to variables
	bsIN.Read(getInputData(userName, sizeof(userName), false));
	bsIN.Read(getInputData(password, sizeof(password), false));
	
	if (inArray(userName, connectedUserList))
	{
		std::cout << "The user " << userName << " has already connected " << std::endl;
		bs.Write(loginSuccessful);
		rpInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
		return;
	}

	//Let the server know who's establishing a connection.
	std::cout << userName << " is connecting." << std::endl;
	userDetails.userName = userName;
	std::string user(userName);
	std::string pass(password);

	bool returncheck = false;

	//If file does exist
	if (fileExists("..\\Users\\" + user + ".txt"))
	{

		//Create and write
		std::fstream userFile("..\\Users\\" + user + ".txt",
			std::ios::in);

		std::string textLineCheck;

		//Check files password against entered password
		//Check file line by line
		for (int i = 0; !userFile.eof(); i++)
		{
			getline(userFile, textLineCheck);
			//If textline == password.
			if (textLineCheck == password)
			{
				returncheck = true;
				std::cout << "Login successful." << std::endl;
				//Let the server know who's establishing a connection.
				std::cout << userName << " has connected." << std::endl;
				loginSuccessful = true;

				//Write all relevant data if login was succesful.
				if (loginSuccessful)
				{
					int num;
					userDetails.loginSuccessful = true;
					connectedUserList.push_back(userName);
					bs.Write(loginSuccessful);
					num = connectedUserList.size();
					bs.Write(num);
					for (int i = 0; i < connectedUserList.size(); ++i)
					{
						bs.Write(RakNet::RakString(connectedUserList[i].c_str()));
					}
					rpInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
					return;
				}
			}//this is correct stop fucking changing it you idiot.
			//all passwords are stored on the 2nd file line,
			//if it hits the line and its not correct login failed.
			else if( i == 1)
			{
				std::cout << "Login Failed: Incorrect Password" << std::endl;
				if (!loginSuccessful)
				{
					//userDetails.loginSuccessful = true;
					bs.Write(loginSuccessful);
					rpInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
					return;
				}
			}
		}
	}
	//If the account doesnt exist.
	else
	{
		std::cout << "Account does not exist: Creating account - " << userName << std::endl;
		//If file does not exist create and write it.
		std::fstream userFile;
		userFile.open("..\\users\\" + user + ".txt", std::ios_base::out);

		//Save user details.
		userDetails.userName = userName;
		userDetails.password = password;
		userDetails.loginSuccessful = true;
		bs.Write(userDetails.loginSuccessful);
		rpInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
		//If file is sucessfully open write username and password to file.
		if (userFile.is_open())
		{
			userFile << userName << std::endl;
			userFile << password << std::endl;
			returncheck = true;
			userFile.close();
		}
	}
}

//Print user data.
void ChatManager::printUserText(RakNet::Packet * packet)
{
	//Read user data and print it.
		RakNet::RakString userRS;
		RakNet::RakString messageRS;
		RakNet::BitStream bs(packet->data, packet->length, false);
		bs.IgnoreBytes(sizeof(RakNet::MessageID));

		bs.Read(userRS);
		bs.Read(messageRS);

		if (messageRS.C_String() != "")
		{
			std::cout << userRS.C_String() << ": " << messageRS.C_String() << std::endl;
		}
}

//Login check for console.
void ChatManager::consoleClientLoginCheck()
{
	bool returncheck = false;
	while (!returncheck)
	{
	std::cout << "Do you have an account? (Y/N)" << std::endl;
	std::string check;

	std::cin >> check;

	//If they have an account.
	if (check == "Y" || check == "y")
	{	
		//Start login procedure.
			std::cout << "Login: " << std::endl;
			std::cout << "User Name: ";
			std::cin >> userDetails.userName;
				
			std::string textLineCheck;

			std::cout << "Password: ";
			std::cin >> userDetails.password;

			returncheck = true;		
	}
	else if (check == "N" || check == "n")
	{
		bool checkin = false;

		std::cout << "Create a new account: " << std::endl;
		
		while (!checkin)
		{
			std::cout << "Username?"; std::cin >> userDetails.userName;
			checkin = true;
		}

		std::cout << "Password?"; std::cin >> userDetails.password;
		returncheck = true;
	}
  }
}

//Takes a char pointer and organises the input
char* ChatManager::getInputData(char * str, int num, bool check)
{
	//If you want to input and not just organise a char pointer
	if (check)
	{
		fgets(str, num, stdin);
	}
	//Sort through string and return it.
	if (str[0] == '\n' || str[0] == '\r')
		str[0] = 0;

	size_t len = strlen(str);
	if (len>0 && (str[len - 1] == '\n' || str[len - 1] == '\r'))
		str[len - 1] = 0;
	if (len>1 && (str[len - 2] == '\n' || str[len - 2] == '\r'))
		str[len - 2] = 0;

	return str;
}

//Creates a new server instance.
void ChatManager::createServer(int maxConnections, const char* ip, const unsigned short port)
{
	m_PORT = port;
	
	//Start up the server.
	std::cout << "Starting server..." << std::endl;

	//Initialise the raknet peerInterface
	rpInterface = RakNet::RakPeerInterface::GetInstance();

	//Create socket descriptor to describe the connection
	RakNet::SocketDescriptor socketDesc(port, ip);

	//Startup
	rpInterface->Startup(maxConnections, &socketDesc, 1);
	rpInterface->SetMaximumIncomingConnections(maxConnections);
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)ID_LOGIN_PROCEDURE);
	rpInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

//Checks if file exists in the current path.
bool ChatManager::fileExists(const std::string& filename)
{
	struct stat buf;
	if (stat(filename.c_str(), &buf) != -1)
	{
		return true;
	}
	return false;
}

//Checks if a member is in an array.
bool ChatManager::inArray(const std::string & value, const std::vector<std::string>& array)
{
	return std::find(array.begin(), array.end(), value) != array.end();
}
