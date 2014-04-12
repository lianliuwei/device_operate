#include "wave_control/osc_wave_group/osc_wave_group.h"

#include <algorithm>

using namespace std;

void OscWaveGroup::AddTriggerObserver(OscWaveGroupObserver* observer) {
  trigger_observer_.AddObserver(observer);
}

void OscWaveGroup::RemoveTriggerObserver(OscWaveGroupObserver* observer) {
  trigger_observer_.RemoveObserver(observer);
}

bool OscWaveGroup::HasTriggerObserver(OscWaveGroupObserver* observer) {
  return trigger_observer_.HasObserver(observer);
}

void OscWaveGroup::AddHorizontalObserver(OscWaveGroupObserver* observer) {
  horizontal_observer_.AddObserver(observer);
}

void OscWaveGroup::RemoveHorizontalObserver(OscWaveGroupObserver* observer) {
  horizontal_observer_.RemoveObserver(observer);
}

bool OscWaveGroup::HasHorizontalObserver(OscWaveGroupObserver* observer) {
  return horizontal_observer_.HasObserver(observer);
}

void OscWaveGroup::AddVerticalObserver(OscWaveGroupObserver* observer) {
  vertical_observer_.AddObserver(observer);
}

void OscWaveGroup::RemoveVerticalObserver(OscWaveGroupObserver* observer) {
  vertical_observer_.RemoveObserver(observer);
}

bool OscWaveGroup::HasVerticalObserver(OscWaveGroupObserver* observer) {
  return vertical_observer_.HasObserver(observer);
}

void OscWaveGroup::NotifyTriggerGroupChanged() {
  FOR_EACH_OBSERVER(OscWaveGroupObserver, trigger_observer_, OnPartGroupChanged());
}

void OscWaveGroup::NotifyTriggerChanged(int id) {
  FOR_EACH_OBSERVER(OscWaveGroupObserver, trigger_observer_, OnPartChanged(id));
}

void OscWaveGroup::NotifyTriggerMoved(int id) {
  FOR_EACH_OBSERVER(OscWaveGroupObserver, trigger_observer_, OnPartMoved(id));
}

void OscWaveGroup::NotifyHorizontalGroupChanged() {
  FOR_EACH_OBSERVER(OscWaveGroupObserver, horizontal_observer_, OnPartGroupChanged());
}

void OscWaveGroup::NotifyHorizontalChanged(int id) {
  FOR_EACH_OBSERVER(OscWaveGroupObserver, horizontal_observer_, OnPartChanged(id));

}

void OscWaveGroup::NotifyHorizontalMoved(int id) {
  FOR_EACH_OBSERVER(OscWaveGroupObserver, horizontal_observer_, OnPartMoved(id));

}

void OscWaveGroup::NotifyVerticalGroupChanged() {
  FOR_EACH_OBSERVER(OscWaveGroupObserver, vertical_observer_, OnPartGroupChanged());

}

void OscWaveGroup::NotifyVerticalChanged(int id) {
  FOR_EACH_OBSERVER(OscWaveGroupObserver, vertical_observer_, OnPartChanged(id));
}

void OscWaveGroup::NotifyVerticalMoved(int id) {
  FOR_EACH_OBSERVER(OscWaveGroupObserver, vertical_observer_, OnPartMoved(id));
}

OscWaveGroup::~OscWaveGroup() {
  DCHECK(triggers_.size() == 0);
  DCHECK(horizontals_.size() == 0);
  DCHECK(verticals_.size() == 0);
}




