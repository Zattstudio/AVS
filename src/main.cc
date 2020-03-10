#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <microhttpd.h>
#include "AudioHandler.h"
#include "Server.h"

#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[0;31m"
#define COLOR_GREEN "\033[0;32m"

#define PORT 8080
#define MIMETYPE "text/html; charset=utf-8"

AudioHandler* handler;

int playlistLength;
int voteRound = 0;

std::vector<int> votes;

bool running = true;
std::stringstream buffer;

std::vector<int> choices;


std::string template_file;


void resetVotes(){
	votes.at(0) = 0;
	votes.at(1) = 0;
	votes.at(2) = 0;
}

bool votesEmpty(int minvotes){ //note: inverted name, should be "votesNotEmpty"
	for (int i = 0; i < votes.size(); ++i)
	{
		if(votes.at(i) >= minvotes){
			return true;
		}
	}
	return false;
}


std::string getSongChoices(){
	return "ÄÄÄÄÄÄÄÄÄÄ," + handler->getFileByIndex(choices.at(0)) + ","
		+ handler->getFileByIndex(choices.at(1)) + ","
		+ handler->getFileByIndex(choices.at(2));
}


int answer_to_connection (void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls){
	

	//std::clog << url << std::endl;
	struct MHD_Response *response;
	std::string url_text = url;


	if (int pos = url_text.find("vote/") != std::string::npos ){
		const char *page  = "";
		int voteId = (int)url[6] -48;
		//failsafe
		if (voteId < 0 || voteId > 2){}
		else{
			votes.at(voteId) = votes.at(voteId) + 1;
			std::clog << "vote incoming: " << voteId << std::endl;
		}

		response = MHD_create_response_from_buffer (strlen (page),(void*) page, MHD_RESPMEM_PERSISTENT);
	}
	else if (url_text.find("rnd.txt") != std::string::npos ){
		const char *page = std::to_string(voteRound).c_str();
		response = MHD_create_response_from_buffer (strlen (page),(void*) page, MHD_RESPMEM_PERSISTENT);
	}
	else if (url_text.find("lit.txt") != std::string::npos ){
		std::string s = getSongChoices().c_str();
		const char *page = s.c_str();
		response = MHD_create_response_from_buffer (strlen (page),(void*) page, MHD_RESPMEM_PERSISTENT);
	}
	else{
	// Handler

		const char *page  = template_file.c_str();
		response = MHD_create_response_from_buffer (strlen (page),(void*) page, MHD_RESPMEM_PERSISTENT);
	}
	int ret;

	MHD_add_response_header(response, "Content-Type", MIMETYPE);
	ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
	MHD_destroy_response (response);
	return ret;
}



std::vector<int> pickSongs()
{
	choices.clear();
	srandom(time(NULL) );
	std::vector<int> choices;
	for(int i =0; i < 3; i++){

		choices.push_back(rand() % playlistLength);
	}
	return choices;
}


int main (int argc, char *argv[])
{

	/* Check for the amt of arguments*/
	if (argc < 3){
		std::cerr << "Usage: " << argv[0] << " [directory] [min. votes]\nQuitting ... \n";
		return 0;
	}


	std::cout << COLOR_GREEN <<"AudioVoteSystem Started ..." << std::endl;
	std::clog << "Audio Handler initialising ..." << std::endl;

	int minvotes = (int)argv[2][0]-48;
	
	handler = new AudioHandler(argv[1]);


	std::clog << COLOR_RED << "Loading songs from directory ...\n" << COLOR_RESET << std::endl;

	handler->loadSongNames();
	handler->printSongList();

	playlistLength = handler->getListLength();

	//load tempalte
	std::ifstream file("data/template.html");

	buffer << file.rdbuf();
	template_file = buffer.str();


	// start http server

	struct MHD_Daemon *daemon;
	daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL, &answer_to_connection, NULL, MHD_OPTION_END);
	if (NULL == daemon) return 1;

	// start main loop

	// init votes
	votes.push_back(0);
	votes.push_back(0);
	votes.push_back(0);
	
	int nextSongIndex = 20;
	int lastSongIndex;

	while (running){


		//await votes
		choices = pickSongs();
		// Generate song choices 
		std::clog << "choices: " << choices.at(0) << ", " << choices.at(1) << ", " << choices.at(2) << std::endl;

		//start voting
		std::clog << COLOR_GREEN << "Voting has startet ..." << COLOR_RESET << std::endl;

		if (nextSongIndex != lastSongIndex){

			handler->playByIndex(nextSongIndex);
			lastSongIndex = nextSongIndex;
			//avoid endless loop or repetition
		}
		//evalualte votes
		if (!votesEmpty(minvotes)) {
			resetVotes();
			std::clog << "Votes resetted ... \n";
			// too low amt of votes 
			handler->playRandomSong();

		}
		else{

			int bestvote = 0;
			int lastentry = 0;
			for(int i=0; i < votes.size(); i++){
				if (votes.at(i) > lastentry){
					bestvote = i;
					lastentry = votes.at(i);
				}

			}
			//evaluated votes
			resetVotes();
			std::clog << "Song " << choices.at(bestvote) << " has won the vote!\n";
			std::clog << "votes resetted ... \n";
			nextSongIndex = choices.at(bestvote);
		}


		//play song

		voteRound++;
	}
	MHD_stop_daemon (daemon);

	return 0;
}
