#include "GaudiKernel/DeclareFactoryEntries.h"
#include "BeamInfoSvc/BeamInfoSvc.h"

DECLARE_SERVICE_FACTORY(BeamInfoSvc)

DECLARE_FACTORY_ENTRIES(BeamInfoSvc) { DECLARE_SERVICE(BeamInfoSvc); }
