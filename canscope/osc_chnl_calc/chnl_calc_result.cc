#include "canscope/osc_chnl_calc/chnl_calc_result.h"


namespace canscope {

ChnlCalcResult::ChnlCalcResult(OscRawDataHandle handle)
    : handle_(handle) {
}

OscRawData* ChnlCalcResult::raw_data() {
  return handle_.get();
}

void ChnlCalcResult::AddCalcDelegateRef() {
  AddRef();
}

void ChnlCalcResult::ReleaseCalcDelegateRef() {
  Release();
}


} // namespace canscope
