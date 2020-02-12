#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
#include "EventModel/EventHeader.h"
#include "BeamInfoSvc/BeamInfoSvc.h"
#include <algorithm>

using CLHEP::HepLorentzVector;
using namespace Event;
using std::cout;
using std::endl;
BeamInfoSvc::BeamInfoSvc(const std::string& name, ISvcLocator* svcLoc)
    : Service(name, svcLoc), m_runID(0), m_eventID(0), m_status(0) {
    UpdateAvialInfo();
    // calibrated beam Energy
    SetName("");
    declareProperty("UseCbE", m_usecbE = true);
}

BeamInfoSvc::~BeamInfoSvc() {}

void BeamInfoSvc::UpdateAvialInfo() {
    // AvailableInfo::Clear();
    AvailableInfo::Add("runID", "int");
    AvailableInfo::Add("eventID", "int");
    AvailableInfo::Add("Ecm", "double");
}
void BeamInfoSvc::GetInfoI(const std::string& info_name, int& targe) {
    // cout << "Info in BeamInfoSvc::GetInfoI: "
    //    << "info_name = " << info_name << endl;
    this->AnaBeamStatus();
    if (info_name == "runID") {
        targe = m_runID;
    } else if (info_name == "eventID") {
        targe = m_eventID;
    }
}
void BeamInfoSvc::GetInfoD(const std::string& info_name, double& targe) {
    if (info_name == "Ecm") {
        this->AnaBeamStatus();
        targe = m_Ecm;
    }
}

void BeamInfoSvc::AnaBeamStatus() {
    SmartDataPtr<Event::EventHeader> eventHeader(eventSvc_,
                                                 "/Event/EventHeader");
    if (m_runID == eventHeader->runNumber() &&
        m_eventID == eventHeader->eventNumber()) {
        //  cout << "Info in BeamInfoSvc::AnaBeamStatus: "
        //      << "#run = " << m_run << ", #id = " << m_event << endl;
        return;
    } else {
        m_runID = eventHeader->runNumber();
        m_eventID = eventHeader->eventNumber();
        //  cout << "Info in BeamInfoSvc::AnaBeamStatus: "
        //      << "#run = " << m_run << ", #id = " << m_event << endl;
    }
    this->ReadDb(m_runID, m_Ecm);
}

void BeamInfoSvc::ReadDb(int run, double& Ecm) {
    Gaudi::svcLocator()->service("DatabaseSvc", m_dbsvc, true);
    // calibrated beam Energy
    if (m_usecbE) {
        char stmt1[1024];
        snprintf(stmt1, 1024,
                 "select beam_energy, px, py, pz "
                 "from RunParams where run_number = %d",
                 run);
        DatabaseRecordVector res;
        int row_no = m_dbsvc->query("offlinedb", stmt1, res);
        if (row_no == 0) {
            m_status = 401;
            return;
        }
        DatabaseRecord* records = res[0];
        Ecm = 2 * records->GetDouble("beam_energy");
    } else {
        // use online beam Energy
        char stmt1[1024];
        snprintf(stmt1, 1024,
                 "select BER_PRB, BPR_PRB "
                 "from RunParams where run_number = %d",
                 run);
        DatabaseRecordVector res;
        int row_no = m_dbsvc->query("run", stmt1, res);
        if (row_no == 0) {
            m_status = 402;
            return;
        }
        DatabaseRecord* records = res[0];
        double E_E, E_P;
        E_E = records->GetDouble("BER_PRB");
        E_P = records->GetDouble("BPR_PRB");
        Ecm = E_E + E_P;
    }
    return;
}

void BeamInfoSvc::GetEcm(double& Ecm) {
    AnaBeamStatus();
    if (m_status == 0) {
        Ecm = m_Ecm;
    }
}
