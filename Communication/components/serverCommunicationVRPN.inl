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
    std::vector<vrpn_Button_Server*> sendersButton;
    std::vector<vrpn_Tracker_Server*> sendersTracker;

    std::string address = d_address.getValueString();

    //Creating Server
    //Sending text via VRPN
    vrpn_Connection *m_connection = vrpn_create_server_connection();

    //For tracker
    // We will just put a fake data in the position of our tracker
    static float angle = 0;
    angle += 0.001f;
    vrpn_float64 pos[3], d_quat[4];

    // the pos array contains the position value of the tracker
    // XXX Set your values here
    pos[0] = sin ( angle );
    pos[1] = 0.0f;
    pos[2] = 0.0f;

    // the d_quat array contains the orientation value of the tracker, stored as a quaternion
    // XXX Set your values here
    d_quat[0] = 0.0f;
    d_quat[1] = 0.0f;
    d_quat[2] = 0.0f;
    d_quat[3] = 1.0f;

    struct timeval delay;
    delay.tv_usec = 0L;
    delay.tv_sec = 0L;

    const vrpn_uint32 class_of_service = vrpn_CONNECTION_LOW_LATENCY;
    int sensor = 0;

    //For Button
    int numbuttons = 10;

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
        std::string strTestText = subscriber->getSubject()+"@"+address;
        const char *device = strTestText.c_str();

        //Text Server
        vrpn_Text_Sender *textServer = new vrpn_Text_Sender(device, m_connection);
        sendersText.push_back(textServer);

        //Analog Server
        vrpn_Analog_Server *analogServer = new vrpn_Analog_Server(device, m_connection);
        analogServer->setNumChannels(1);
        sendersAnalog.push_back(analogServer);

//        //Button Server
//        vrpn_Button_Server *buttonServer = new vrpn_Button_Server(device, m_connection, 10);
//        sendersButton.push_back(buttonServer);

        //Tracker Server
        vrpn_Tracker_Server *trackerServer = new vrpn_Tracker_Server(device, m_connection);
        sendersTracker.push_back(trackerServer);
    }

    while (this->m_running)
    {
        while (!m_connection->connected())
        {  // wait until we've got a connection
            m_connection->mainloop();
        }
        while (m_connection->connected())
        {
            printf("Please enter the message:\n");
            for(std::vector<vrpn_Text_Sender*>::iterator it = sendersText.begin(); it != sendersText.end(); it++)
            {
                //For Sending Text
                std::string msgTemp = "***************THIS IS A TEST MESSAGE TO BE RECEIVED AT THE CLIENT****************";
                (*it)->send_message(msgTemp.c_str(), vrpn_TEXT_NORMAL);
                m_connection->mainloop();

            }
        }

        for(std::vector<vrpn_Analog_Server*>::iterator it = sendersAnalog.begin(); it != sendersAnalog.end(); it++)
        {    
            double * channels = (*it)->channels();
            static int done = 0;

            #if 1
            if (!done) 
            {
                channels[0] = 0.0;
                done = 1;
            } else
                channels[0] += 0.5;
            #else
                struct timeval now;
                vrpn_gettimeofday(&now, NULL);
                channels[0] = sin(((double) now.tv_usec) / 1000000L);
            #endif
            (*it)->report_changes();
            m_connection->mainloop(&delay);
            fprintf(stderr, "while():  a = %.2g\n", (*it)->channels()[0]);
        }

//            for(std::vector<vrpn_Button_Server*>::iterator it = sendersButton.begin(); it!= sendersButton.end(); it++)
//            {
//                std::string msgTemp = "********** Sample String to check for Button ***********" ;
//                //String to char*
//                char* msg = const_cast<char*>(msgTemp.c_str());
//                //(*it)->encode_states_to(true );
//                (*it)->mainloop();
//            }

        for(std::vector<vrpn_Tracker_Server*>::iterator it = sendersTracker.begin(); it != sendersTracker.end(); it++)
        {
            (*it)->report_pose(sensor, delay, pos, d_quat, class_of_service);
            (*it)->mainloop();
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
