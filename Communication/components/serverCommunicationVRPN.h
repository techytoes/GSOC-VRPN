#include "serverCommunication.h"

#include<vrpn_Analog.h>
#include<vrpn_Text.h>
#include<vrpn_Connection.h>

#define MAX 1024

namespace sofa
{

namespace component
{

namespace communication
{

class SOFA_COMMUNICATION_API ServerCommunicationVRPN : public ServerCommunication
{
public:

    typedef ServerCommunication Inherited;
    SOFA_CLASS(ServerCommunicationVRPN, Inherited);

    //ServerCommunicationVRPN();
    virtual ~ServerCommunicationVRPN();

    //ArgumentList stringToArgumentList(std::string dataString);

    //////////////////////////////// Factory VRPN type /////////////////////////////////
    typedef CommunicationDataFactory VRPNDataFactory;
    VRPNDataFactory* getFactoryInstance();
    virtual void initTypeFactory() override;
    /////////////////////////////////////////////////////////////////////////////////

    virtual std::string getArgumentType(std::string value) override;
    virtual std::string getArgumentValue(std::string value) override;

    //Data<helper::OptionsGroup>  d_pattern;
    //int getTimeout() const;
    //void setTimeout(int timeout);

protected:

    //////////////////////////////// Inherited from ServerCommunication /////////////////////////////////
    virtual void sendData() override;
    virtual void receiveData() override;
    virtual std::string defaultDataType() override;
    /////////////////////////////////////////////////////////////////////////////////

    //void sendRequest();
    //void receiveRequest();

    static void VRPN_CALLBACK processTextMessage(void *userdata, const vrpn_TEXTCB t);
    //std::string createVRPNMessage(CommunicationSubscriber* subscriber, std::string argument);
    //void processMessage(std::string dataString);

};

}   /// namespace communication
}   /// namespace component
}   /// namespace sofa


