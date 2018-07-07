#include <stdio.h>                      // for printf, NULL, fprintf, etc

#include "vrpn_Analog.h"                // for vrpn_Analog_Remote, etc
#include "vrpn_Analog_Output.h"
#include "vrpn_Button.h"                // for vrpn_Button_Remote, etc
#include "vrpn_Configure.h"             // for VRPN_CALLBACK, etc
#include "vrpn_Connection.h"            // for vrpn_Connection, etc
#include "vrpn_Dial.h"                  // for vrpn_Dial_Remote, etc
#include "vrpn_Poser.h"                 // for vrpn_POSERCB, etc
#include "vrpn_Shared.h"                // for timeval, vrpn_gettimeofday, etc
#include "vrpn_Text.h"                  // for vrpn_Text_Receiver, etc
#include "vrpn_Tracker.h"               // for vrpn_Tracker_Remote, etc
#include "vrpn_Types.h"                 // for vrpn_float64
#include "vrpn_Tracker_Filter.h"
using namespace std;

vrpn_Button_Example_Server	*sbtn;
const char	*BUTTON_NAME = "testing@localhost";
vrpn_Connection		*connection;

int main(int argc, char const *argv[])
{
	sbtn = new vrpn_Button_Example_Server(BUTTON_NAME, connection);
	printf("Button's name is %s.\n", BUTTON_NAME);	


    while(1) {
        sbtn->mainloop();
        connection->mainloop();
        fprintf(stderr, "Server Open");
    }

}
