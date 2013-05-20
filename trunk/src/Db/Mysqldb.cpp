/*
 * Mysqldb.cpp
 *
 *  Created on: 28 avr. 2013
 *      Author: poppy
 */

#include "Mysqldb.h"

MYSQL Mysqldb::mysql;

Mysqldb::Mysqldb() {
	// TODO Auto-generated constructor stub

}

bool Mysqldb::Exec(string requete) {
	if (!DryRun) {

		if (!isConnected) {
			if (!Connect()) {
				return false;
			}
		}

		if (mysql_query(&mysql, requete.c_str())) {
			log(LOG_ERR, "Erreur INSERT %d: \%s \n", mysql_errno(&mysql),
					mysql_error(&mysql));
			return false;
		} else {
			log(LOG_INFO, "Requete MySql ok.");
			return true;
		}
	} else {
		log(LOG_ERR, requete);
		return true;
	}
}

bool Mysqldb::Connect() {
	if (isConnected) {
		return true;
	} else {
		/* INIT MYSQL AND CONNECT ----------------------------------------------------*/
		if (!mysql_init(&mysql)) {
			log(LOG_ERR, "Erreur: Initialisation MySQL impossible !");
			return false;
		}
		if (!mysql_real_connect(&mysql, Mysql_HOST.c_str(),
				Mysql_LOGIN.c_str(), Mysql_PWD.c_str(), Mysql_DB.c_str(), 0,
				NULL, 0)) {
			log(LOG_ERR, "Erreur connection %d: %s \n", mysql_errno(&mysql),
					mysql_error(&mysql));
			return false;
		}

		isConnected = true;
		return true;
	}
}

void Mysqldb::DisConnect() {
	if (isConnected) {
		mysql_close(&mysql);
		isConnected = false;
	}
}

Mysqldb Mysqldb::m_instance = Mysqldb();

Mysqldb& Mysqldb::Instance() {
	return m_instance;
}
