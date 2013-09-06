#include "base/debug/trace_event_impl.h"
#include "base/files/file_path.h"
#include "base/file_util.h"

class ScopedTraceEvent {
public:
  ScopedTraceEvent(const char* filter, const base::FilePath& file_path);
  ~ScopedTraceEvent();

private:
  void OnTraceDataCollected(
      const scoped_refptr<base::RefCountedString>& events_str);
  
  base::FilePath file_path_;

  DISALLOW_COPY_AND_ASSIGN(ScopedTraceEvent);
};
