#include "canscope/device/canscope_data_collecter_callback.h"

#include "base/string_number_conversions.h"

namespace canscope {

void OscDataCollected(OscRawDataHandle raw_data) {
  LOG(INFO) << "Get one Raw Data"
    << " detail: " << base::HexEncode(raw_data->data() + 1000, 200);
}

} // namespace canscope