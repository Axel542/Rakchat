#pragma once
#include "ChatManager.h"

int main()
{
		
	//Create chat manager instance
	auto cm = new ChatManager();
	//Create server instance.SS
	cm->createServer(32, "127.0.0.1", 5456);
	
	std::string comm;

	//Threads to handle network messages and commands.
	std::vector<std::thread *> threads;
	threads.push_back(new std::thread(&ChatManager::handleServerNetworkMessages, cm, cm->getRPInterface()));
	threads.push_back(new std::thread(&ChatManager::handleServerCommands, cm, 0, comm));

	for (auto i = 0; i < threads.size(); ++i)
		{
			if (threads[i]->joinable())
			{
				threads[i]->join();
			}
			delete threads[i];
		}
	
	delete cm;
	return 0;
}