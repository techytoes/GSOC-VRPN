#ifndef SOFA_CONTROLLER_ServerCommunicationVRPN_INL
#define SOFA_CONTROLLER_ServerCommunicationVRPN_INL

#include<serverCommunicationVRPN.h>

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
    m_socket = new vrpn_Text_Reciever(address);

}
}
}
}
