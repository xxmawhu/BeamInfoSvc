#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "EventModel/EventHeader.h"
#include "BeamInfoSvc/BeamInfoSvc.h"
#include <algorithm>

using CLHEP::HepLorentzVector;
using namespace Event;
using std::cout;
using std::endl;
StatusCode BeamInfoSvc::initialize() {
    MsgStream log(messageService(), name());
    log << MSG::INFO << "@initialize()" << endreq;

    StatusCode sc = Service::initialize();
    sc = serviceLocator()->service("EventDataSvc", eventSvc_, true);
    return sc;
}

StatusCode BeamInfoSvc::finalize() {
    MsgStream log(messageService(), name());
    log << MSG::INFO << "@initialize()" << endreq;
    StatusCode sc = Service::finalize();
    return sc;
}

StatusCode BeamInfoSvc::queryInterface(const InterfaceID& riid, void** ppvIF) {
    if (IBeamInfoSvc::interfaceID().versionMatch(riid)) {
        *ppvIF = dynamic_cast<IBeamInfoSvc*>(this);
    } else {
        return Service::queryInterface(riid, ppvIF);
    }
    addRef();
    // cout<<"BeamInfoSvc::Inf:queryInterface"<<endl;
    return StatusCode::SUCCESS;
}
