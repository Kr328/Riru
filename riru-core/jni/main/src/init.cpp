#include <string>

#include "log.h"
#include "hooker.h"
#include "utils.h"
#include "modules.h"

extern "C" void inject(void) __attribute__((constructor));

//Run on library loaded
void inject(void) {
    Log::info() << string("[Init] Inject") << Log::END;

    //Check app_process process
    try {
        Log::info() << "[Init] " << "Check process executable" << Log::END;
        string current     = Utils::getCurrentExecutablePath();
        string app_process = Utils::resolveRealPath(APP_PROCESS_PATH);

        if ( current != app_process ) {
            Log::info() << "[Init] " << current << " not app_process. Exit." << Log::END;
            return ;
        }
    }
    catch (exception const &e ) {
        Log::error() << "[Init] " << "Check process failure: " << e.what() << Log::END;
        return ;
    }

    //Install hooks for replace native methods
    try {
        Log::info() << "[Init] " << "Install hook" << Log::END;
        Hooker::install();
    }
    catch (exception const &e) {
        Log::error() << "[Init] " << "install hook failure: " << e.what() << Log::END;
        return;
    }

    //Load all modules
    try {
        Log::info() << "[Init] " << "Initial modules" << Log::END;
        Modules::get().loadAll();
        Modules::get().refreshCache();
        Modules::get().invokeAllModuleLoaded();
    }
    catch (exception const &e) {
        Log::error() << "[Init] " << "Initial modules failure. " << e.what() << Log::END;
        return ;
    }

    Log::info() << "[Init] " << "Successfully Inject" << Log::END;
    return;
}
