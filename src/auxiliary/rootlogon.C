#include <TInterpreter.h>
#include <TSystem.h>


/**
 * Add the content of rootlogon() to your function in rootlogon.C.
 */
void rootlogon()
{
    /**
     * Modify this to the installed path of ROOT_helper
     */
    const char* ROOT_helper_include_path = "-I/usr/local/include/ROOT_helper";

    gInterpreter->AddIncludePath(ROOT_helper_include_path);
    gSystem->AddIncludePath(ROOT_helper_include_path);
}
