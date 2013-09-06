#include "canscope/scoped_trace_event.h"

ScopedTraceEvent::ScopedTraceEvent(const char* filter, 
                                   const base::FilePath& file_path) 
    : file_path_(file_path) {
  using namespace base::debug;
  TraceLog::GetInstance()->SetEnabled(CategoryFilter(filter), 
    TraceLog::RECORD_UNTIL_FULL);
}

ScopedTraceEvent::~ScopedTraceEvent() {
  using namespace base::debug;
  TraceLog::GetInstance()->SetDisabled();
  TraceLog::GetInstance()->Flush(
      base::Bind(&ScopedTraceEvent::OnTraceDataCollected,
      base::Unretained(this)));
}

void ScopedTraceEvent::OnTraceDataCollected(
    const scoped_refptr<base::RefCountedString>& events_str) {
  using namespace base::debug;

  TraceResultBuffer::SimpleOutput json_output;
  TraceResultBuffer trace_buffer;

  trace_buffer.SetOutputCallback(json_output.GetCallback());
  trace_buffer.Start();
  trace_buffer.AddFragment(events_str->data());
  trace_buffer.Finish();

  int ret = file_util::WriteFile(file_path_, 
    json_output.json_output.c_str(), json_output.json_output.size());
  DLOG_IF(ERROR, ret < 0) << "save trace event error";
}
