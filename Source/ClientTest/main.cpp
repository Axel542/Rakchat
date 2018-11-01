#include "ChatManager.h"

int main()
{
	//create chat manager
	auto cm = new ChatManager();
	//Start connection to server.
	cm->startClientConnection("127.0.0.1", 5456);

	//first login check.
	cm->consoleClientLoginCheck();
	cm->sendUserData();
	std::string comm;

	std::vector<std::thread*> threads;


	//Threads to handle network messages and commands.
	threads.push_back(new std::thread(&ChatManager::handleClientNetworkMessages, cm, cm->getRPInterface()));
	threads.push_back(new std::thread(&ChatManager::receiveInputData, cm));

	for (auto i = 0; i < threads.size(); ++i)
	{
		if (threads[i]->joinable())
		{
			threads[i]->join();
		}

		delete threads[i];
	};

	delete cm;
	return 0;
}