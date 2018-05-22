#ifndef SOFA_CONTROLLER_ServerCommunicationVRPN_INL
#define SOFA_CONTROLLER_ServerCommunicationVRPN_INL

#include<serverCommunicationVRPN.h>
#include<vrpn_Text.h>
#include<vrpn_Connection.h>

namespace sofa
{

namespace component
{

namespace communication
{

void ServerCommunicationVRPN::receiveData()
{
    std::string ip = "localhost";
    std::string device = "Mouse0";
    std::string address = ip+device;
    vrpn_Text_Reciever *m_socket = new vrpn_Text_Reciever(address);
    m_socket->register_message_handler(0, handle_text);

    while(1)
    {
        m_socket->mainloop();
    }
}
}
}
}
