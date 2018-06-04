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
    return "string";
}

/******************************************************************************
*                                                                             *
* SEND PART                                                                   *
*                                                                             *
******************************************************************************/

void ServerCommunicationVRPN::sendData()
{
    std::vector<vrpn_Text_Sender*> sendersText;
    std::vector<vrpn_Analog_Server*> sendersAnalog;

    std::string address = d_address.getValueString();

    //Creating Server
    //Sending text via VRPN
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

        //Text Server
        vrpn_Text_Sender *textServer = new vrpn_Text_Sender(device, sc);
        sendersText.push_back(textServer);

        //Analog Server
        vrpn_Analog_Server *analogServer = new vrpn_Analog_Server(device, sc);
        sendersAnalog.push_back(analogServer);

        //Button Server
    }

    while (this->m_running)
    {
        while (!sc->connected())
        {  // wait until we've got a connection
            sc->mainloop();
        }
        while (sc->connected())
        {
            //printf("Please enter the message:\n");
            for(std::vector<vrpn_Text_Sender*>::iterator it = sendersText.begin(); it != sendersText.end(); it++)
            {
                //For Sending Text
                std::string msgTemp = "***************THIS IS A TEST MESSAGE TO BE RECEIVED AT THE CLIENT****************";
                (*it)->send_message(msgTemp.c_str(), vrpn_TEXT_NORMAL);
                (*it)->mainloop();

            }

            for(std::vector<vrpn_Analog_Server*>::iterator it = sendersAnalog.begin(); it != sendersAnalog.end(); it++)
            {
                (*it)->report_changes();
            }

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
    std::vector<vrpn_BaseClass*> receivers;

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

        //Recieving Text via VRPN
        vrpn_Text_Receiver *vrpnText = new vrpn_Text_Receiver(device);
        vrpnText->register_message_handler( (void*)subscriber->getSubject().c_str(), processTextMessage );
        receivers.push_back(vrpnText);

        //Receiving Analog via VRPN
        vrpn_Analog_Remote *vrpnAnalog = new vrpn_Analog_Remote(device);
        vrpnAnalog->register_change_handler( (void*)subscriber->getSubject().c_str(), processAnalogMessage);
        receivers.push_back(vrpnAnalog);

        //Receiving Button via VRPN
        vrpn_Button_Remote *vrpnButton = new vrpn_Button_Remote(device);
        vrpnButton->register_change_handler( (void*)subscriber->getSubject().c_str(), processButtonMessage);
        receivers.push_back(vrpnButton);

        //Receiving Tracker via VRPN
        vrpn_Tracker_Remote *vrpnTracker = new vrpn_Tracker_Remote(device);
        vrpnTracker->register_change_handler( (void*)subscriber->getSubject().c_str(), processTrackerMessage);
        receivers.push_back(vrpnTracker);
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
        //processs see linefunction processmessaage of osc
    }
}

void VRPN_CALLBACK ServerCommunicationVRPN::processAnalogMessage(void *userdata, const vrpn_ANALOGCB a)
{
    int nbChannels = a.num_channel;
    std::cout << "Analog : ";
    for( int i=0; i < a.num_channel; i++ )
    {
        std::cout << a.channel[i] << " ";
    }
    std::cout<<std::endl;
}

void VRPN_CALLBACK ServerCommunicationVRPN::processButtonMessage(void *userdata, const vrpn_BUTTONCB b)
{
    std::cout << "Button '" << b.button << "': " << b.state << std::endl;
}

void VRPN_CALLBACK ServerCommunicationVRPN::processTrackerMessage(void *userdata, const vrpn_TRACKERCB z)
{
    std::cout << "Tracker '" << z.sensor << "': " << z.pos[0] << "," <<  z.pos[1] << "," << z.pos[2] << std::endl;
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