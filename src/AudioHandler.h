#ifndef AUDIO_HANDLER_H_
#define AUDIO_HANDLER_H_

#include <iostream>
#include <vector>
#include <string>
#include <dirent.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h> 
#include <time.h>


enum HandlerStatus
{
	PLAYING,
	STOPPED,
	PAUSED
};

class AudioHandler {
	public:
		int status = STOPPED;
		AudioHandler(char* dirpath); 
		~AudioHandler();
		std::string getFileByIndex(int index);
		void playRandomSong();
		void playByIndex(int index);
		int getListLength();

		void loadSongNames();
		void printSongList();
	private:
		std::vector<std::string> m_Songs;
		int result = 0;
		int flags = MIX_INIT_MP3;
		Mix_Music *music;
		char* m_DirectoryPath;


};


#endif

