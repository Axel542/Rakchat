//THIS file is for storing deprecated code that may be reimplemented at a later date.


//top of the h file
struct Lobby
{
	std::string lobbyName;
	std::string chatLog[5096];
	int lobbyIndex;
};


//protected from the h file

std::list<Lobby> lobbyList;
	Lobby connectedLobby;
	
//public from the h file.

void createLobby(Lobby a_lobby, std::string lobbyName);

//Saves lobbies when server closes.
	void saveLobbies();

	//Loads lobbies when server starts.
	void loadLobbies();
	
//from the cpp file.

void ChatManager::createLobby(Lobby a_lobby, std::string lobbyName)
{
	a_lobby.lobbyName = lobbyName;
	lobbyList.push_back(a_lobby);
	
}


void ChatManager::saveLobbies()
{
	std::list<Lobby>::iterator IT;

	for (IT = lobbyList.begin(); IT != lobbyList.end(); ++IT)
	{
		if (fileExists("..\\Lobbies\\" + IT->lobbyName + ".txt"))
		{
			//Create and write
			std::fstream userFile("..\\Lobbies\\" + IT->lobbyName + ".txt",
				std::ios::in);
		}
		else
		{
			std::fstream userFile;
			userFile.open("..\\Lobbies\\" + IT->lobbyName + ".txt", std::ios_base::out);

			if (userFile.is_open())
			{
				userFile << IT->lobbyName << std::endl;
				userFile << IT->lobbyIndex << std::endl;
			}
		}
	}
}

void ChatManager::loadLobbies()
{
}

//from handleserverCommands create lobby command.
							Lobby newLobby;
							std::cout << "New lobby name?" << std::endl;
							std::cin >> newLobby.lobbyName;

							createLobby(newLobby, newLobby.lobbyName);
							saveLobbies();
							
					