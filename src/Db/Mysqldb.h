/*
 * Mysqldb.h
 *
 *  Created on: 28 avr. 2013
 *      Author: poppy
 */
#include <iostream>
#include <list>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include "../global.h"
#include <mysql/mysql.h>

using namespace std;

#ifndef MYSQLDB_H_
#define MYSQLDB_H_

class Mysqldb {

public:
	Mysqldb();

	bool Exec(string requete);

	void DisConnect();

	static Mysqldb& Instance();
private:
	static MYSQL mysql;

	bool Connect();

	bool isConnected;

	static Mysqldb m_instance;

};

#endif /* MYSQLDB_H_ */
