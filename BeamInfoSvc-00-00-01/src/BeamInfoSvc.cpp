/* ====================================================
#   Copyright (C)2020 All rights reserved.
#
#   Author        : Xin-Xin MA
#   Email         : xxmawhu@163.com
#   File Name     : BeamInfoSvc.cpp
#   Create Time   : 2020-02-13 10:44
#   Last Modified : 2020-02-13 10:44
#   Describe      :
#Debug tip:

* replace "///" with ";;;"
  %s%///%;;;
* recovery
  %s%;;;%///
#
# ====================================================*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
#include "EventModel/EventHeader.h"
#include "BeamInfoSvc/BeamInfoSvc.h"
#include "BesStdSelector/Selector.h"
#include <algorithm>

using CLHEP::HepLorentzVector;
using namespace Event;
using std::cout;
using std::endl;
using namespace BesStdSelector;
BeamInfoSvc::BeamInfoSvc(const std::string& name, ISvcLocator* svcLoc)
    : Service(name, svcLoc), m_runID(0), m_eventID(0), 
    m_status(0), 
    m_setEcm(0) {
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
    /// cout << "Info in BeamInfoSvc::GetInfoI: ";
    /// cout << "info_name = " << info_name << endl;
    this->AnaBeamStatus();
    if (info_name == "runID") {
        targe = m_runID;
    } else if (info_name == "eventID") {
        targe = m_eventID;
    }
}
void BeamInfoSvc::GetInfoD(const std::string& info_name, double& targe) {
    if (info_name == "Ecm") {
        GetEcm(targe);
    }
}

void BeamInfoSvc::AnaBeamStatus() {
    SmartDataPtr<Event::EventHeader> eventHeader(eventSvc_,
                                                 "/Event/EventHeader");
    if (m_runID == eventHeader->runNumber() &&
        m_eventID == eventHeader->eventNumber()) {
        /// cout << "Info in BeamInfoSvc::AnaBeamStatus: ";
        /// cout << "#run = " << m_runID << ", #id = " << m_eventID << endl;
        return;
    } else {
        m_runID = eventHeader->runNumber();
        m_eventID = eventHeader->eventNumber();
        /// cout << "Info in BeamInfoSvc::AnaBeamStatus: ";
        /// cout << "#run = " << m_runID << ", #id = " << m_eventID << endl;
    }
    this->ReadDb(abs(m_runID), m_Ecm);
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
       /// cout << "calibrated beam_energy = " << records->GetDouble("beam_energy");
       /// cout << endl;
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
        /// cout << "beam_energy = " << Ecm << endl;
    }
    return;
}

void BeamInfoSvc::GetEcm(double& Ecm) {
    AnaBeamStatus();
    if (m_setEcm) {
        Ecm = m_Ecm;
    }
    if (m_status == 0) {
        Ecm = m_Ecm;
    }
}
void BeamInfoSvc::SetEcm(double Ecm) {
    m_setEcm = true;
    m_Ecm = Ecm;
}

void BeamInfoSvc::AnaTrackStatus() {
    SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc_,
                                          "/Event/EvtRec/EvtRecEvent");
    SmartDataPtr<EvtRecTrackCol> evtRecTrackCol(eventSvc_,
                                                "/Event/EvtRec/EvtRecTrackCol");
    if (BesStdSelector::soloPhotonSelector.vetoPi0()) {
        SmartDataPtr<EvtRecPi0Col> recPi0Col(eventSvc_,
                                             "/Event/EvtRec/EvtRecPi0Col");
        CDPi0List Pi0List(pi0Selector);
        dc_fill(Pi0List, recPi0Col->begin(), recPi0Col->end());
        /// cout << "Info in <PhotonConverSvc::GetParList>: ";
        /// cout << "Pi0List.size = " << Pi0List.size() << endl;

        // fill into  the vector
        vector<const EvtRecPi0*> _pi0s;
        for (CDPi0List::iterator itr = Pi0List.particle_begin();
             itr != Pi0List.particle_end(); ++itr) {
            const EvtRecPi0* navPi0 = (*itr).particle().navPi0();
            _pi0s.push_back(navPi0);
        }
        BesStdSelector::soloPhotonSelector.setPi0s(_pi0s);
    }

    int nCharged = evtRecEvent->totalCharged();
    m_tracks = nCharged;
    EvtRecTrackIterator chr_begin = evtRecTrackCol->begin();
    EvtRecTrackIterator chr_end = evtRecTrackCol->begin() + nCharged;
    EvtRecTrackIterator neu_begin = evtRecTrackCol->begin() + nCharged;
    EvtRecTrackIterator neu_end = evtRecTrackCol->end();
    m_showers = neu_end - neu_begin;
    ///  cout << "Info in <PhotonConverSvc::GetParList>: ";
    ///  cout << "#showers = " << neu_end - neu_begin << endl;
    CDPhotonList m_PhotonList = CDPhotonList(neu_begin, neu_end, 
                        BesStdSelector::soloPhotonSelector);
    CDChargedPionList m_primaList  =  CDChargedPionList(chr_begin, chr_end,
            BesStdSelector::PrimaryGoodChrTrkSelector);
    m_primaryTrks = m_primaList.size();
    CDChargedPionList m_secondList  =  CDChargedPionList(chr_begin, chr_end,
            BesStdSelector::SecondaryGoodChrTrkSelector);
    m_secondTrks = m_secondList.size();
    /// cout << "Info in <PhotonConverSvc::GetParList>: ";
    /// cout << "#solo photon = " << m_PhotonList.size() << endl;
    return;
}
