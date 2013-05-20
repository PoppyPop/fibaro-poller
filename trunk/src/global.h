/*
 * global.h
 *
 *  Created on: 18 févr. 2013
 *      Author: poppy
 */

#include <iostream>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <sys/syslog.h>
#include <cstdarg>

using namespace std;

#ifndef GLOBAL_H_
#define GLOBAL_H_

extern bool ModeDebug;
extern bool ModeDeamon;
extern bool ModeTest;
extern bool ModeVisu;
extern bool DryRun;

extern string Mysql_HOST;
extern string Mysql_DB;
extern string Mysql_TABLE;
extern string Mysql_LOGIN;
extern string Mysql_PWD;

void log(int priorite, string message, ...);

string format(string message, ...);

string prepareSqlTxtParam(string valeur);

string prepareSqlTxtParam(int valeur);

string prepareSqlTxtParam(double valeur);

#endif /* GLOBAL_H_ */
