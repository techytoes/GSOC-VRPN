#include "serverCommunication.h"

#include<vrpn_Analog.h>
#include<vrpn_Text.h>
#include<vrpn_Connection.h>

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

    ServerCommunicationVRPN();
    virtual ~ServerCommunicationVRPN();

    ArgumentList stringToArgumentList(std::string dataString);

    //////////////////////////////// Factory VRPN type /////////////////////////////////
    typedef CommunicationDataFactory VRPNDataFactory;
    VRPNDataFactory* getFactoryInstance();
    virtual void initTypeFactory() override;
    /////////////////////////////////////////////////////////////////////////////////

    virtual std::string getArgumentType(std::string value) override;
    virtual std::string getArgumentValue(std::string value) override;

    Data<helper::OptionsGroup>  d_pattern;


    int getTimeout() const;
    void setTimeout(int timeout);

protected:

    //zmq::context_t     m_context{1};
    vrpn_Text_Receiver      *m_socket;
    //int                m_timeout;

    //////////////////////////////// Inherited from ServerCommunication /////////////////////////////////
    virtual void sendData() override;
    virtual void receiveData() override;
    virtual std::string defaultDataType() override;
    /////////////////////////////////////////////////////////////////////////////////

    void sendRequest();
    void receiveRequest();

    std::string createVRPNMessage(CommunicationSubscriber* subscriber, std::string argument);
    void processMessage(std::string dataString);

};

}   /// namespace communication
}   /// namespace component
}   /// namespace sofa

#endif // SOFA_SERVERCOMMUNICATIONVRPN_H
