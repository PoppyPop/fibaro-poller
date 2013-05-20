/*
 * fibaro-temp.h
 *
 *  Created on: 22 avr. 2013
 *      Author: poppy
 */
#include <iostream>
#include <list>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include "global.h"
#include <mysql/mysql.h>
#include "Db/Mysqldb.h"

using namespace std;

#ifndef FIBAROTEMP_H_
#define FIBAROTEMP_H_

class fibarotemps {
public:
	bool Enregistrer(fibarotemps & reftemps);

	bool Enregistrer(int piece, int id);

	bool VaEnregistrer(fibarotemps reftemps);

	fibarotemps();

	bool AjoutTemp(int piece, int id, string nom, double temp);

	bool EstRenseigner();

	double GetTemp(int piece, int id);

private:

	class fibarotemp {
	public:
		fibarotemp();

		void Set(int piece, int id, string nom, double temp);

		int GetPiece();
		int GetId();
		string GetNom();
		double GetTemp();

		bool IsNull();

		string ToString();

		bool EstEnregistrer();

		bool Enregistrer(string dateTime);
	private:
		int m_piece;
		int m_id;
		string m_nom;
		double m_temp;

		bool isNull;

		bool estEnregistrer;
	};



	time_t datetime;

	list<fibarotemp> temps;

	bool TempExiste(int piece, int id);

	string GetDate();

	fibarotemp GetFibaroTemp(int piece, int id);
};

#endif /* FIBAROTEMP_H_ */
