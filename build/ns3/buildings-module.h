
#ifdef NS3_MODULE_COMPILATION
# error "Do not include ns3 module aggregator headers from other modules; these are meant only for end user scripts."
#endif

#ifndef NS3_MODULE_BUILDINGS
    

// Module headers:
#include "building-allocator.h"
#include "building-container.h"
#include "building-list.h"
#include "building-position-allocator.h"
#include "building.h"
#include "buildings-channel-condition-model.h"
#include "buildings-helper.h"
#include "buildings-propagation-loss-model.h"
#include "hybrid-buildings-propagation-loss-model.h"
#include "itu-r-1238-propagation-loss-model.h"
#include "mobility-building-info.h"
#include "oh-buildings-propagation-loss-model.h"
#include "random-walk-2d-outdoor-mobility-model.h"
#include "three-gpp-v2v-channel-condition-model.h"
#endif
