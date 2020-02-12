/* ====================================================
#   Copyright (C)2020 All rights reserved.
#
#   Author        : Xin-Xin MA
#   Email         : xxmawhu@163.com
#   File Name     : BeamInfoSvc.h
#   Create Time   : 2020-02-08 15:07
#   Last Modified : 2020-02-08 15:07
#   Describe      :
#
# ====================================================*/
#ifndef BeamInfoSvc_BeamInfoSvc_H
#define BeamInfoSvc_BeamInfoSvc_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "McTruth/McParticle.h"
#include "McTruth/MdcMcHit.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "BeamInfoSvc/IBeamInfoSvc.h"
#include "EventNavigator/EventNavigator.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "DatabaseSvc/IDatabaseSvc.h"
#include <mysql.h>
#include "McDecayModeSvc/McDecayModeSvc.h"

#include "HadronInfo/AvailableInfo.h"
#include <map>
#include <vector>
using CLHEP::Hep3Vector;
using CLHEP::Hep2Vector;
using CLHEP::HepLorentzVector;
using std::vector;
using Event::McParticle;
using Event::McParticleCol;
using Event::MdcMcHitCol;

class BeamInfoSvc : public Service,
                    virtual public IBeamInfoSvc,
                    public AvailableInfo {
   public:
    BeamInfoSvc(const std::string& name, ISvcLocator* svcLoc);
    virtual ~BeamInfoSvc();
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);

    virtual void GetInfoI(const std::string& info_name, int& targe);
    virtual void GetInfoD(const std::string& info_name, double& targe);
    void GetEcm(double& Ecm);
    void SetEcm(double Ecm);

   private:
    bool m_usecbE, m_setEcm;
    int m_runID, m_eventID;
    int m_status;
    double m_Ecm;

    IDataProviderSvc* eventSvc_;
    IDatabaseSvc* m_dbsvc;
    // McDecayModeSvc* m_MCDecayModeSvc;
    mutable EventNavigator* m_navigator;
    void ReadDb(int run, double&);
    void AnaBeamStatus();
    void UpdateAvialInfo();
};
#endif
