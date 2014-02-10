#pragma once

#include "canscope/device/osc_raw_data.h"

namespace canscope {

// now only one device allow, so just using one static DataCollected callback
// unitests and product use different implement.
void OscDataCollected(OscRawDataHandle raw_data);

} // namespace canscope