/* Branch Monitor 
 * Introspection Client
 * Marcus Botacin, 2017
 */

#pragma once
#include<stdio.h>
#include<Windows.h>
#include <signal.h>
#include<string.h>
#include<queue>
#include<mutex>

/* The max number of allowed libs for the introspection procedure
 * This should be changed to a malloc version in order to grow
 * When using Launcher.py, argv size is the upper bound 
 */
#define MAX_LIBS 100

