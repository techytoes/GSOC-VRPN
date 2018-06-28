/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2017 INRIA, USTL, UJF, CNRS, MGH                    *
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
#define WIN32_LEAN_AND_MEAN

#include <SofaTest/Sofa_test.h>
using sofa::Sofa_test ;

#include <sofa/simulation/Node.h>
using sofa::simulation::Node ;
using sofa::core::ExecParams;

#include <sofa/core/objectmodel/Base.h>
using sofa::core::objectmodel::Base;

#include <sofa/core/objectmodel/Data.h>
using sofa::core::objectmodel::Data;
using sofa::core::objectmodel::BaseData;

#include <sofa/helper/vectorData.h>
using sofa::helper::vectorData;

#include <SofaSimulationCommon/SceneLoaderXML.h>
using sofa::simulation::SceneLoaderXML ;

#include <sofa/simulation/Simulation.h>
#include <SofaSimulationGraph/init.h>
#include <SofaSimulationGraph/DAGSimulation.h>
using sofa::simulation::Simulation ;

#include <sofa/core/ObjectFactory.h>
using sofa::core::ObjectFactory ;

// COMMUNICATION PART
#include <Communication/components/serverCommunication.h>
#include <Communication/components/serverCommunicationVRPN.h>
#include <Communication/components/CommunicationSubscriber.h>
using sofa::component::communication::ServerCommunication;
using sofa::component::communication::ServerCommunicationVRPN;
using sofa::component::communication::CommunicationSubscriber;

//VRPN TEST PART
#include <vrpn_Connection.h>
#include <vrpn_Analog.h>
#include <vrpn_Text.h>

// TIMEOUT
#include <iostream>
#include <future>
#include <thread>
#include <chrono>

namespace sofa
{
namespace component
{
namespace communication
{

class Communication_testVRPN : public Sofa_test<>
{

public:

    /// BASIC STUFF TEST
    /// testing subscriber + argument creation VRPN

    void checkAddSubscriber()
    {
        std::stringstream scene1 ;
        scene1 <<
                  "<?xml version='1.0' ?>                                                       \n"
                  "<Node name='root'>                                                           \n"
                  "   <DefaultAnimationLoop/>                                                   \n"
                  "   <RequiredPlugin name='Communication' />                                   \n"
                  "   <ServerCommunicationVRPN name='vrpnSender' job='sender' port='6000'  refreshRate='1000'/> \n"
                  "   <CommunicationSubscriber name='sub1' communication='@vrpnSender' subject='/test' target='@vrpnSender' datas='x'/>"
                  "</Node>                                                                      \n";

        Node::SPtr root = SceneLoaderXML::loadFromMemory ("testscene", scene1.str().c_str(), scene1.str().size()) ;
        root->init(ExecParams::defaultInstance()) ;

        ServerCommunication* aServerCommunicationVRPN = dynamic_cast<ServerCommunication*>(root->getObject("vrpnSender"));
        std::map<std::string, CommunicationSubscriber*> map = aServerCommunicationVRPN->getSubscribers();
        EXPECT_EQ(map.size(), 1);
    }

    void checkGetSubscriber()
    {
        std::stringstream scene1 ;
        scene1 <<
                  "<?xml version='1.0' ?>                                                       \n"
                  "<Node name='root'>                                                           \n"
                  "   <DefaultAnimationLoop/>                                                   \n"
                  "   <RequiredPlugin name='Communication' />                                   \n"
                  "   <ServerCommunicationVRPN name='vrpnSender' job='sender' port='6000'  refreshRate='1000'/> \n"
                  "   <CommunicationSubscriber name='sub1' communication='@vrpnSender' subject='/test' target='@vrpnSender' datas='x'/>"
                  "</Node>                                                                      \n";

        Node::SPtr root = SceneLoaderXML::loadFromMemory ("testscene", scene1.str().c_str(), scene1.str().size()) ;
        root->init(ExecParams::defaultInstance()) ;

        ServerCommunication* aServerCommunicationVRPN = dynamic_cast<ServerCommunication*>(root->getObject("vrpnSender"));
        CommunicationSubscriber* subscriber = aServerCommunicationVRPN->getSubscriberFor("/test");
        EXPECT_NE(subscriber, nullptr) ;
    }

    void checkArgumentCreation()
    {
        std::stringstream scene1 ;
        scene1 <<
                  "<?xml version='1.0' ?>                                                       \n"
                  "<Node name='root'>                                                           \n"
                  "   <DefaultAnimationLoop/>                                                   \n"
                  "   <RequiredPlugin name='Communication' />                                   \n"
                  "   <ServerCommunicationVRPN name='vrpnSender' job='sender' port='6000' refreshRate='1000'/> \n"
                  "   <CommunicationSubscriber name='sub1' communication='@vrpnSender' subject='/test' target='@vrpnSender' datas='x'/>"
                  "</Node>                                                                      \n";

        Node::SPtr root = SceneLoaderXML::loadFromMemory ("testscene", scene1.str().c_str(), scene1.str().size()) ;
        root->init(ExecParams::defaultInstance());

        ServerCommunication* aServerCommunicationVRPN = dynamic_cast<ServerCommunication*>(root->getObject("vrpnSender"));
        EXPECT_NE(aServerCommunicationVRPN, nullptr);

        for(unsigned int i=0; i<10; i++)
            sofa::simulation::getSimulation()->animate(root.get(), 0.01);

        Base::MapData dataMap = aServerCommunicationVRPN->getDataAliases();
        Base::MapData::const_iterator itData;
        BaseData* data;

        itData = dataMap.find("port");
        EXPECT_TRUE(itData != dataMap.end());
        if (itData != dataMap.end())
        {
            data = itData->second;
            EXPECT_NE(data, nullptr) ;
        }

        itData = dataMap.find("x");
        EXPECT_TRUE(itData != dataMap.end());
        if (itData != dataMap.end())
        {
            data = itData->second;
            EXPECT_NE(data, nullptr) ;
        }
    }

    void checkCreationDestruction()
    {
        std::stringstream scene1 ;
        scene1 <<
                  "<?xml version='1.0' ?>                                                       \n"
                  "<Node name='root'>                                                           \n"
                  "   <DefaultAnimationLoop/>                                                   \n"
                  "   <RequiredPlugin name='Communication' />                                   \n"
                  "   <ServerCommunicationVRPN name='vrpnSender' job='sender' port='6000'  refreshRate='1000'/> \n"
                  "</Node>                                                                      \n";

        Node::SPtr root = SceneLoaderXML::loadFromMemory ("testscene", scene1.str().c_str(), scene1.str().size()) ;
        root->init(ExecParams::defaultInstance()) ;
    }

    void checkSendReceiveVRPN()
    {

        std::stringstream scene1 ;
        scene1 <<
                  "<?xml version='1.0' ?>                                                       \n"
                  "<Node name='root'>                                                           \n"
                  "   <DefaultAnimationLoop/>                                                   \n"
                  "   <RequiredPlugin name='Communication' />                                   \n"
                  "   <ServerCommunicationVRPN name='vrpnSender' job='sender' port='6000'  refreshRate='1000'/> \n"
                  "   <CommunicationSubscriber name='subSender' communication='@vrpnSender' subject='/test' target='@vrpnSender' datas='port'/>"
                  "   <ServerCommunicationVRPN name='vrpnReceiver' job='receiver' port='6000' /> \n"
                  "   <CommunicationSubscriber name='subReceiver' communication='@vrpnReceiver' subject='/test' target='@vrpnReceiver' datas='x'/>"
                  "</Node>                                                                      \n";

        Node::SPtr root = SceneLoaderXML::loadFromMemory ("testscene", scene1.str().c_str(), scene1.str().size()) ;
        root->init(ExecParams::defaultInstance()) ;

        ServerCommunication* aServerCommunicationVRPNSender = dynamic_cast<ServerCommunication*>(root->getObject("vrpnSender"));
        ServerCommunication* aServerCommunicationVRPNReceiver = dynamic_cast<ServerCommunication*>(root->getObject("vrpnReceiver"));
        EXPECT_NE(aServerCommunicationVRPNSender, nullptr);
        EXPECT_NE(aServerCommunicationVRPNReceiver, nullptr);

        for( int i = 0; i < 100; i++ )
            sofa::simulation::getSimulation()->animate(root.get(),0.01);

        aServerCommunicationVRPNReceiver->setRunning(false);

        Base::MapData dataMap = aServerCommunicationVRPNReceiver->getDataAliases();
        Base::MapData::const_iterator itData;
        BaseData* data;

        itData = dataMap.find("x");
        EXPECT_TRUE(itData != dataMap.end());
        if (itData != dataMap.end())
        {
            data = itData->second;
            EXPECT_NE(data, nullptr) ;
            EXPECT_STRCASEEQ(data->getValueString().c_str(), "6000") ;
        }
    }
};

TEST_F(Communication_testVRPN, checkAddSubscriber) {
    ASSERT_NO_THROW(this->checkAddSubscriber()) ;
}

TEST_F(Communication_testVRPN, checkGetSubscriber) {
    ASSERT_NO_THROW(this->checkGetSubscriber()) ;
}

TEST_F(Communication_testVRPN, checkArgumentCreation) {
    ASSERT_NO_THROW(this->checkArgumentCreation()) ;
}

TEST_F(Communication_testVRPN, checkCreationDestruction) {
    ASSERT_NO_THROW(this->checkCreationDestruction()) ;
}

TEST_F(Communication_testVRPN, checkSendReceiveVRPN) {
    ASSERT_NO_THROW(this->checkSendReceiveVRPN()) ;
}

} // communication
} // component
} // sofa
