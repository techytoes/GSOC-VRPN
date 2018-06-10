#include "vrpn_Button.h"

#include <iostream>
using namespace std;

void VRPN_CALLBACK handle_button( void* userData, const vrpn_BUTTONCB b )
{
    cout << "Button '" << b.button << "': " << b.state << endl;
}

int main(int argc, char* argv[])
{
    vrpn_Button_Remote* vrpnButton = new vrpn_Button_Remote("Mouse0@localhost");

    vrpnButton->register_change_handler( 0, handle_button );

    while(1)
    {
        vrpnButton->mainloop();
    }

    return 0;
}