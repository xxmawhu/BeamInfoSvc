/* ====================================================
#   Copyright (C)2020 All rights reserved.
#
#   Author        : Xin-Xin MA
#   Email         : xxmawhu@163.com
#   File Name     : IBeamInfoSvc.h
#   Create Time   : 2020-02-08 15:07
#   Last Modified : 2020-02-08 15:07
#   Describe      :
#
# ====================================================*/
#ifndef BeamInfoSvc__IBeamInfoSvc_H
#define BeamInfoSvc__IBeamInfoSvc_H
#include "GaudiKernel/IService.h"
#include "McTruth/McParticle.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "CLHEP/Vector/LorentzVector.h"

/* Decaration of the interface ID */
static const InterfaceID IID_IBeamInfoSvc("IBeamInfoSvc", 1, 0);

class IBeamInfoSvc : virtual public IService {
   public:
    virtual ~IBeamInfoSvc() {}
    static const InterfaceID& interfaceID() { return IID_IBeamInfoSvc; }
};
#endif
