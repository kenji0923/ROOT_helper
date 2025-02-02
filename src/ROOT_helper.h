/**
 * Include this header for usage in ROOT cling interpreter.
 * Used only for an installed package.
 * Add following lines to rootlogon.C to add include path
 *         const char* ROOT_helper_include_path = "-I/usr/local/include/ROOT_helper";
 *         gInterpreter->AddIncludePath(ROOT_helper_include_path);
 *         gSystem->AddIncludePath(ROOT_helper_include_path);
 */


#ifndef ROOT_HELPER_H
#define ROOT_HELPER_H


#define ROOT_HELPER_USED_IN_INTERPRETER


#include "data_saver.h"
#include "src/data_saver.cpp"

#include "graphics.h"
#include "src/graphics.cpp"

#include "container.h"
#include "src/container.cpp"

#include "analysis.h"
#include "src/analysis.cpp"


#endif
