#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#ifndef _WIN32
#include <unistd.h>
#include <sys/time.h>
#endif
#include <vrpn_Connection.h>
#include "vrpn_Text.h"
#include "vrpn_Analog.h"

#define MAX 1024

int main ()
{
	char msg[MAX];
	vrpn_Connection *sc = vrpn_create_server_connection();
	vrpn_Text_Sender *s = new vrpn_Text_Sender("Mouse0@localhost", sc);
	
	while (1) {
		while (!sc->connected()) {  // wait until we've got a connection
		  sc->mainloop();
                }
                while (sc->connected()) {
		  printf("Please enter the message:\n");
		  if (scanf("%s", msg) != 1) {
			fprintf(stderr, "No message entered\n");
			return(-1);
		  }
	          s->send_message(msg, vrpn_TEXT_NORMAL);
		  sc->mainloop();
		}
	}
}
