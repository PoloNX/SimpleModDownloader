#include <filesystem>
#include <iostream>
#include <string>

#include <switch.h>

#define PATH				"/switch/SimpleModDownloader/"
#define APP_PATH			"/switch/SimpleModDownloader.nro"
#define BAD_APP_PATH		"/switch/SimpleModDownloader/SimpleModDownloader.nro"
#define FORWARDER_PATH	  	"/config/SimpleModDownloader/forwarder.nro"
#define APP_DOWNLOAD_PATH 	"/config/SimpleModDownloader/app.nro"

int main()
{   
	socketInitializeDefault();
    nxlinkStdio();
	std::cout<< "CREATE DIRECTORY PATH" << std::endl;
	consoleUpdate(NULL);
	sleep(1);
	std::filesystem::create_directory(PATH);

	if(std::filesystem::exists(APP_DOWNLOAD_PATH)) {
		std::cout<< "RENAME DOWNLOAD PATH" << std::endl;
		consoleUpdate(NULL);
		sleep(1);
		std::filesystem::remove(APP_PATH);
		std::filesystem::rename(APP_DOWNLOAD_PATH, APP_PATH);
		if(std::filesystem::exists(APP_DOWNLOAD_PATH))
			std::filesystem::remove(APP_DOWNLOAD_PATH);
	}

	if (std::filesystem::exists(BAD_APP_PATH)) {
		std::cout<< "REMOVE BAD APP PATH" << std::endl;
		consoleUpdate(NULL);
		sleep(1);
		std::filesystem::remove(BAD_APP_PATH);
	}

	std::cout << "REMOVES" << std::endl;
	consoleUpdate(NULL);
		sleep(1);
	if(std::filesystem::exists(FORWARDER_PATH))
		std::filesystem::remove(FORWARDER_PATH);

	
	std::cout << "ENV" << std::endl;
	consoleUpdate(NULL);
	sleep(1);
	envSetNextLoad("switch/SimpleModDownloader.nro", "\"/switch/SimpleModDownloader.nro\"");
	std::cout << "RETURN" << std::endl;
	consoleUpdate(NULL);
    socketExit();
	sleep(1);
	return 0;
}
