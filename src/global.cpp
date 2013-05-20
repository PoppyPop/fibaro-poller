/*
 * global.cpp
 *
 *  Created on: 18 févr. 2013
 *      Author: poppy
 */
#include "global.h"

// Gestion des params
bool ModeDebug = false;
bool ModeDeamon = false;
bool ModeTest = false;
bool ModeVisu = false;
bool DryRun = false;

// Define mysql
string Mysql_HOST = "localhost";
string Mysql_DB = "temperatures";
string Mysql_TABLE = "temperatures";
string Mysql_LOGIN = "temperatures";
string Mysql_PWD = "temperatures";

void log(int priorite, string message, ...) {
	va_list args;
	va_start(args, message);

	char buffer[1024 * 16];
	vsnprintf(buffer, (1024 * 16) - 1, message.c_str(), args);

	if (ModeDebug) {
		printf("%s\n", buffer);
	} else if (priorite != LOG_INFO && ModeVisu) {
		printf("%s\n", buffer);
	}

	// Par defaut pas de log des INFOS
	if (priorite != LOG_INFO || ModeDebug) {

		syslog(priorite, buffer);
	}
}

string format(string message, ...) {
	va_list args;
	va_start(args, message);

	char buffer[1024 * 16];
	vsnprintf(buffer, (1024 * 16) - 1, message.c_str(), args);

	return buffer;
}

string prepareSqlTxtParam(string valeur) {

	std::ostringstream oss; // string as stream

	oss << "'" << valeur << "'"; //

	return oss.str();
}

string prepareSqlTxtParam(int valeur) {

	std::ostringstream oss; // string as stream

	oss << valeur; //

	return oss.str();
}

string prepareSqlTxtParam(double valeur) {

	std::ostringstream oss; // string as stream

	oss << valeur; //

	return oss.str();
}
