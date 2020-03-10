#include "AudioHandler.h"

AudioHandler::AudioHandler(char* dirpath){
	// set rnd seed once
	srand( time(0) );		
	// Init sdl and chatch errors
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to init SDL\n";
    }

    if (flags != (result = Mix_Init(flags))) {
        std::cerr << "Could not initialize mixer.\n";
	}
	this->m_DirectoryPath = dirpath;

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
	Mix_VolumeMusic(90);
}

AudioHandler::~AudioHandler(){
	m_Songs.clear();
	Mix_FreeMusic(music);
	SDL_Quit();
}


std::string AudioHandler::getFileByIndex(int index){
	if (m_Songs.size() > 0)
		return m_Songs.at(index);
}



void AudioHandler::playRandomSong(){
	int randIndex = rand() % m_Songs.size();
	playByIndex(randIndex);
}
void AudioHandler::playByIndex(int index){

	status = PLAYING;

	std::string songpath = m_DirectoryPath + m_Songs.at(index);

	std::clog << "\033[0;33mPlaying Song: " << songpath << "\033[0m\n";

    music = Mix_LoadMUS(songpath.c_str());
    Mix_PlayMusic(music, 0);


    while (Mix_PlayingMusic()) {
        SDL_Delay(250);
	}
	status = STOPPED;
}


int AudioHandler::getListLength(){
	return this->m_Songs.size();
}


void AudioHandler::loadSongNames(){

    DIR*    dir;
    dirent* pdir;

    dir = opendir(m_DirectoryPath);

    while (pdir = readdir(dir)) {
    	// c solution for reading directory
    	if ( !strcmp(pdir->d_name, ".") || !strcmp(pdir->d_name, "..") )
		{
     		// do nothing here
		} else {
			m_Songs.push_back(pdir->d_name);
		}
    }
}

void AudioHandler::printSongList(){
	for(int i=m_Songs.size()-1; i >= 0; i-- ){
		std::cout << i << ": " << m_Songs.at(i) << std::endl;
	}
	std::cout << "-------------\n";
}

