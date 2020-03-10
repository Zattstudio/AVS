#ifndef SERVER_H_
#define SERVER_H_

#include <iostream>
#include <string>
#include <vector>
#include <microhttpd.h>


#define SERVER_PORT 8080


class Server{
	public:
		Server();
		~Server();
	private:
		int answerToConnection (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls);
};



#endif