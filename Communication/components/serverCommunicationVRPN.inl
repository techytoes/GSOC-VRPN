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
#include <serverCommunicationVRPN.h>
#include <Communication/components/CommunicationSubscriber.h>

#include <vrpn_Text.h>
#include <vrpn_Connection.h>

namespace sofa
{

namespace component
{

namespace communication
{

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

/******************************************************************************
*                                                                             *
* RECEIVE PART                                                                *
*                                                                             *
******************************************************************************/

void ServerCommunicationVRPN::receiveData()
{
    std::string address = d_address.getValueString();
    bool status=false;
    std::vector<vrpn_Text_Receiver*> receivers;
    std::map<std::string, CommunicationSubscriber*> subscribersMap = getSubscribers();
    if (subscribersMap.size() == 0)
        continue;
    for (std::map<std::string, CommunicationSubscriber*>::iterator it = subscribersMap.begin(); it != subscribersMap.end(); it++)
    {
        CommunicationSubscriber* subscriber = it->second;
        vrpn_Text_Receiver *a = new vrpn_Text_Receiver(subscriber->getSubject()+"@"+address);
        a->register_message_handler( subscriber->getSubject(), processTextMessage );
        receivers.push_back(a);
    }

    while(!status)
    {
        for(auto rec : receivers )
        {
            rec->mainloop();
        }
    }
}

void VRPN_CALLBACK ServerCommunicationVRPN::processTextMessage(void *userdata, const vrpn_TEXTCB t)
{
    const char *name = (const char *)userdata;
    if (t.type == vrpn_TEXT_NORMAL)
    {
        ArgumentList messageStream;
        messageStream.push_back("string:"+t.message);
        printf("%s: Text message: %s\n", name, t.message);
        //processs see linefunciton processmessaage of osc
    }
}
}
}
}