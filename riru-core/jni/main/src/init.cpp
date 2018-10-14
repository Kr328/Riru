#include <string>

#include "log.h"
#include "hooker.h"
#include "utils.h"
#include "modules.h"

extern "C" void inject(void) __attribute__((constructor));

//Run on library loaded
void inject(void) {
    Log::i << "Inject" << Log::END;

    //Check app_process process
    try {
        string current     = Utils::getCurrentExecutablePath();
        string app_process = Utils::resolveRealPath(APP_PROCESS_PATH);

        if ( current != app_process ) {
            Log::i << current << " not app_process. Exit." << Log::END;
            return ;
        }
    }
    catch (exception const &e ) {
        Log::e << "check process failure: " << e.what() << Log::END;
        return ;
    }

    //Load all modules
    try {
        Modules::get().loadAll();
        Modules::get().refreshCache();
    }
    catch (exception const &e) {
        Log::e << "Load modules failure. " << e.what() << Log::END;
        return ;
    }

    //Install hooks for replace native methods
    try {
        Hooker::install();
    }
    catch (exception const &e) {
        Log::e << "install hook failure: " << e.what() << Log::END;
        return;
    }

    Log::i << "Inject success." << Log::END;
    return;
}
