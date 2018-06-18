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
    std::string address = d_address.getValueString();
    m_connection = vrpn_create_server_connection();
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
        std::string strTestText = subscriber->getSubject()+"@"+address;
        const char *device = strTestText.c_str();
        vrpn_text_sender = new vrpn_Text_Sender(device, m_connection);
        vrpn_analog_server = new vrpn_Analog_Server(device, m_connection);

    }

    while (!m_connection->connected() && this->m_running)
        m_connection->mainloop();

    while (m_connection->connected() && this->m_running)
    {

        for (std::map<std::string, CommunicationSubscriber*>::iterator it = subscribersMap.begin(); it != subscribersMap.end(); it++)
        {
            CommunicationSubscriber* subscriber = it->second;
            ArgumentList argumentList = subscriber->getArgumentList();

            try
            {
                for (ArgumentList::iterator itArgument = argumentList.begin(); itArgument != argumentList.end(); itArgument++ )
                    sendVRPNMessage(subscriber, *itArgument);
            } catch(const std::exception& e) {
                if (isVerbose())
                    msg_info("ServerCommunicationVRPN") << e.what();
            }
            m_connection->mainloop(&delay);
        }
    }
}

void ServerCommunicationVRPN::sendVRPNMessage(CommunicationSubscriber* subscriber, std::string argument)
{
    std::stringstream messageStr;
    BaseData* data = fetchDataFromSenderBuffer(subscriber, argument);
    if (!data)
        throw std::invalid_argument("data is null");
    const AbstractTypeInfo *typeinfo = data->getValueTypeInfo();
    const void* valueVoidPtr = data->getValueVoidPtr();

    delay.tv_sec = 0L;
    delay.tv_usec = 0L;

    if (!typeinfo->Container())
    {
        if (vrpn_text_sender)
            vrpn_text_sender->send_message(data->getValueString().c_str(), vrpn_TEXT_NORMAL);

        if (vrpn_analog_server)
        {
            vrpn_analog_server->setNumChannels(1);
            double *channels = vrpn_analog_server->channels();
            static int done = 0;
            if (!done)
            {
                //converting string to double*
                channels[0] = stod (data->getValueString());
                done = 1;
            }
            else
                channels[0] += stod (data->getValueString());

            vrpn_analog_server->report_changes();
        }
    }

    delete data;
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
    ArgumentList textStream;
    ServerCommunicationVRPN obj;
    std::map<std::string, CommunicationSubscriber*> subscribersMap = obj.getSubscribers();
    for (std::map<std::string, CommunicationSubscriber*>::iterator it = subscribersMap.begin(); it != subscribersMap.end(); it++)
    {
        CommunicationSubscriber* subscriber = it->second;
        if (t.type == vrpn_TEXT_NORMAL)
        {
            std::string stream = "string:";
            stream.append(t.message);
            textStream.push_back("VRPNstring" + stream);
        }
        obj.saveDatasToReceivedBuffer(subscriber->getSubject(), textStream, -1, -1);
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

