RakChat Documentation v0.91

Created by Angus Secomb.

RakChat is a c++ library using RakNet on the backend to provide simple functionality
to setup your own network chat.
RakChat requires the library NetworkChat(RakChat) and the test application is split 
into two projects the server and client.

All user data is saved to the folder Users.
Run server first then start client.

How to use:
Server:
To use the server, just run the servertest.exe.
From here you can kick users, monitor the server,chat to users
from the server console and shut it down.

Commands:
!help - the help function lists the functions below in console.
!kick [userName] - Kicks user from the server
!shutdown - Closes server and application.
!say [message] - Sends global server message.
!online - Online users.

Client:
The client is where users connect to the server.
Users can create an account with a password and join the chat,
they can chat to other users and message them directly as well.


Commands:
!help - the help function lists the functions below in console.
!whisper [userName] - Sends cheeky private message to user.
!exit - Exits application
!logout - logs out to the login prompt.
!online - shows the username of other connected clients.
!connect - connect to a new connection if kicked.

SETTING UP A TEST APPLICATION:

Creating Client MAIN()
To run a client the main must have an instance of the chat manager.
you then call ->startClientConnection("IP", PORT) to connect to the server.
You then call ->consoleClientLoginCheck();
Then ->sendUserData();
create a string for saving commands temporarily to.
create a std::vector<std::thread*> threads to store the two threads needed
to run the program.
push back handleClientNetworkMessages to the thread vector. e.g threads.push_back(new std::thread(&ChatManager::handleClientNetworkMessages, cm, cm->getRPInterface()));
push back receiveInputData to thread vector. e.g threads.push_back(new std::thread(&ChatManager::receiveInputData, cm));
put the threads in a for loop or iterator and the client is done!
for (auto i = 0; i < threads.size(); ++i)
{
	if (threads[i]->joinable())
	{
		threads[i]->join();
	}

	delete threads[i];
};

Creating Server MAIN()
To run the server create an instance of chat manager.
call ->createServer(maxConnections, IP, port);
create a string for saving commands temporarily to. std::string comm;
crate a vector of threads. std::vector<std::thread *> threads;
push back handleServerNetworkMessages and handleServerCommands.
E.G : threads.push_back(new std::thread(&ChatManager::handleServerNetworkMessages, cm, cm->getRPInterface()));
      threads.push_back(new std::thread(&ChatManager::handleServerCommands, cm, 0, comm));

run the threads through a for loop or iterator
e.g
for (auto i = 0; i < threads.size(); ++i)
{
	if (threads[i]->joinable())
	{
		threads[i]->join();
	}
		delete threads[i];
}
The server is now ready to go!



Don't forget to include ChatManager.h and link the appropriate files.
Don't forget to delete your chatmanager instance at the end of your main too!

DEPRECATED FEATURES:
Lobbies are deprecated for now.
!joinLobby [lobbyName] - Joins the lobby.(Client)
!leaveLobby [lobbyName] - Leave lobby (Client)
!createLobby [lobbyName] - Creates new Lobbies.(server)
!destroyLobby [lobbyName] - Destroys lobby.(server)
!usersInLobby [lobbyName] - gets users inside of the lobby.