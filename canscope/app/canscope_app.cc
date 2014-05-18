#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/process_util.h"
#include "base/run_loop.h"
#include "base/i18n/icu_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/ui_base_paths.h"
#include "ui/views/focus/accelerator_handler.h"
#include "ui/base/win/scoped_ole_initializer.h"

#include "canscope/app/canscope_process.h"

using namespace canscope;

int main(int argc, char** argv) {
  ui::ScopedOleInitializer ole_initializer;

  CommandLine::Init(argc, argv);
  logging::InitLogging(NULL,
      logging::LOG_ONLY_TO_SYSTEM_DEBUG_LOG,
      logging::LOCK_LOG_FILE,
      logging::DELETE_OLD_LOG_FILE,
      logging::DISABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS);

  base::EnableTerminationOnHeapCorruption();

  // The exit manager is in charge of calling the dtors of singleton objects.
  base::AtExitManager exit_manager;

  ui::RegisterPathProvider();
  bool icu_result = icu_util::Initialize();
  CHECK(icu_result);
  ui::ResourceBundle::InitSharedInstanceWithLocale("en-US", NULL);

  MessageLoopForUI main_message_loop;

  new CANScopeProcess();
  CANScopeProcess::Get()->Init();

  views::AcceleratorHandler accelerator_handler;
  base::RunLoop run;
  run.set_dispatcher(&accelerator_handler);
  run.Run();

  CANScopeProcess::Destroy();

  return 0;
}