
#ifdef NS3_MODULE_COMPILATION
# error "Do not include ns3 module aggregator headers from other modules; these are meant only for end user scripts."
#endif

#ifndef NS3_MODULE_PROPAGATION
    

// Module headers:
#include "channel-condition-model.h"
#include "cost231-propagation-loss-model.h"
#include "itu-r-1411-los-propagation-loss-model.h"
#include "itu-r-1411-nlos-over-rooftop-propagation-loss-model.h"
#include "jakes-process.h"
#include "jakes-propagation-loss-model.h"
#include "kun-2600-mhz-propagation-loss-model.h"
#include "okumura-hata-propagation-loss-model.h"
#include "probabilistic-v2v-channel-condition-model.h"
#include "propagation-cache.h"
#include "propagation-delay-model.h"
#include "propagation-environment.h"
#include "propagation-loss-model.h"
#include "three-gpp-propagation-loss-model.h"
#include "three-gpp-v2v-propagation-loss-model.h"
#endif
