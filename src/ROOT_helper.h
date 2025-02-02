/**
 * Include this header for usage in ROOT cling interpreter.
 * Used only for an installed package.
 * In order to use in the interpreter, see auxiliary/rootlogon.C
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
