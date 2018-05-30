/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2018 INRIA, USTL, UJF, CNRS, MGH                    *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include "serverCommunicationVRPN.h"
#include "Communication/components/CommunicationSubscriber.h"

#include <vrpn_Text.h>
#include <vrpn_Connection.h>

namespace sofa
{

namespace component
{

namespace communication
{

ServerCommunicationVRPN::~ServerCommunicationVRPN()
{

}

ServerCommunicationVRPN::VRPNDataFactory* ServerCommunicationVRPN::getFactoryInstance()
{
    static VRPNDataFactory* s_localfactory = nullptr ;
    if(s_localfactory==nullptr)
        s_localfactory = new ServerCommunicationVRPN::VRPNDataFactory() ;
    return s_localfactory ;
}

void ServerCommunicationVRPN::initTypeFactory()
{
    getFactoryInstance()->registerCreator("float", new DataCreator<float>());
    getFactoryInstance()->registerCreator("double", new DataCreator<double>());
    getFactoryInstance()->registerCreator("int", new DataCreator<int>());
    getFactoryInstance()->registerCreator("string", new DataCreator<std::string>());

    getFactoryInstance()->registerCreator("matrixfloat", new DataCreator<FullMatrix<float>>());
    getFactoryInstance()->registerCreator("matrixdouble", new DataCreator<FullMatrix<double>>());
    getFactoryInstance()->registerCreator("matrixint", new DataCreator<FullMatrix<int>>());
}

std::string ServerCommunicationVRPN::defaultDataType()
{
    return NULL;
}

/******************************************************************************
*                                                                             *
* SEND PART                                                                   *
*                                                                             *
******************************************************************************/

void ServerCommunicationVRPN::sendData()
{
    std::string address = d_address.getValueString();
    char msg[MAX];
    std::vector<vrpn_Text_Sender*> senders;

    //Creating Server
    vrpn_Connection *sc = vrpn_create_server_connection();

    std::map<std::string, CommunicationSubscriber*> subscribersMap = getSubscribers();
    if (subscribersMap.size() == 0)
    {
        if (isVerbose())
            msg_info(this) << "Server Communication VRPN does not have Subscriber";
        return;
    }
    for (std::map<std::string, CommunicationSubscriber*>::iterator it = subscribersMap.begin(); it != subscribersMap.end(); it++)
    {
        CommunicationSubscriber* subscriber = it->second;

        //Taking a string in converting it into char *
        std::string str = subscriber->getSubject()+"@"+address;
        const char *device = str.c_str();

        //Sending text via VRPN
        vrpn_Text_Sender *s = new vrpn_Text_Sender(device, sc);
        senders.front();
    }

    while (this->m_running)
    {
        while (!sc->connected())
        {  // wait until we've got a connection
            sc->mainloop();
        }
        while (sc->connected())
        {
            printf("Please enter the message:\n");
            if (scanf("%s", msg) != 1)
            {
                fprintf(stderr, "No message entered\n");
                return(-1);
            }
            s->send_message(msg, vrpn_TEXT_NORMAL);
            sc->mainloop();
        }
    }
}

/******************************************************************************
*                                                                             *
* RECEIVE PART                                                                *
*                                                                             *
******************************************************************************/

void ServerCommunicationVRPN::receiveData()
{
    std::string address = d_address.getValueString();
    std::vector<vrpn_Text_Receiver*> receivers;

    std::map<std::string, CommunicationSubscriber*> subscribersMap = getSubscribers();
    if (subscribersMap.size() == 0)
    {
        if (isVerbose())
            msg_info(this) << "Server Communication VRPN does not have Subscriber";
        return;
    }
    for (std::map<std::string, CommunicationSubscriber*>::iterator it = subscribersMap.begin(); it != subscribersMap.end(); it++)
    {
        CommunicationSubscriber* subscriber = it->second;

        //Taking a string in convertng it into char *
        std::string str = subscriber->getSubject()+"@"+address;
        const char *device = str.c_str();

        //Recieving text via VRPN
        vrpn_Text_Receiver *text = new vrpn_Text_Receiver(device);
        text->register_message_handler( (void*)subscriber->getSubject().c_str(), processTextMessage );
        receivers.push_back(text);
    }


    while(this->m_running)
    {
        for(auto rec : receivers )
        {
            rec->mainloop();
        }
    }
}

void VRPN_CALLBACK ServerCommunicationVRPN::processTextMessage(void *userdata, const vrpn_TEXTCB t)
{
    std::cout << "Type : " << t.type << std::endl;
    const char *name = (const char *)userdata;
    if (t.type == vrpn_TEXT_NORMAL)
    {
        ArgumentList messageStream;
        std::string stream = "string:";
        stream.append(t.message);
        messageStream.push_back(stream);
        std::cout << name << " : Text message: " << t.message << std::endl;
        //processs see linefunciton processmessaage of osc
    }
}

std::string ServerCommunicationVRPN::getArgumentValue(std::string value)
{
    return NULL;
}

std::string ServerCommunicationVRPN::getArgumentType(std::string value)
{
    return NULL;
}
}
}
}


