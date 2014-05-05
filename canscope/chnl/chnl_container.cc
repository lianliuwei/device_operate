#include "canscope/chnl/chnl_container.h"

#include <algorithm>

using namespace std;

Chnl* ChnlContainer::GetChnlByType(ChnlTypeEnum type) {
  for (size_t i = 0; i < chnls_.size(); ++i) {
    if (chnls_[i]->ChnlType() == type) {
      return chnls_[i];
    }
  }
  return NULL;
}

int ChnlContainer::ChnlCount() const {
  return chnls_.size();
}

Chnl* ChnlContainer::GetChnlAt(int index) {
  return chnls_[index];
}

int ChnlContainer::ChnlAt(Chnl* chnl) {
  for (size_t i = 0; i < chnls_.size(); ++i) {
    if (chnls_[i] == chnl) {
      return i;
    }
  }
  NOTREACHED();
  return 0;
}

void ChnlContainer::AddChnl(Chnl* chnl) {
  DCHECK(find(chnls_.begin(), chnls_.end(), chnl) == chnls_.end()) << "add twice";
  chnls_.push_back(chnl);
}

void ChnlContainer::AddChnlAt(Chnl* chnl, int index) {
  DCHECK(find(chnls_.begin(), chnls_.end(), chnl) == chnls_.end()) << "add twice";
  chnls_.insert(chnls_.begin() + index, chnl);
}

void ChnlContainer::RemoveChnl(Chnl* chnl) {
  vector<Chnl*>::iterator it = find(chnls_.begin(), chnls_.end(), chnl);
  DCHECK(it != chnls_.end()) << "no add before";
  chnls_.erase(it);
}

void ChnlContainer::RemoveAll() {
  chnls_.clear();
}

ChnlContainer::~ChnlContainer() {
  for (size_t i = 0; i < chnls_.size(); ++i) {
    delete chnls_[i];
  }
  chnls_.clear();
}
