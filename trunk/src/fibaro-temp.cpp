/*
 * fibaro-temp.cpp
 *
 *  Created on: 22 avr. 2013
 *      Author: poppy
 */

#include "fibaro-temp.h"

string fibarotemps::GetDate() {
	char sdate[21];
	struct tm *dc;
	dc = localtime(&datetime);
	// strftime(sdate, sizeof sdate, "%s", dc);
	strftime(sdate, sizeof sdate, "%Y-%m-%d %H:%M:%S", dc);
	return sdate;
}

bool fibarotemps::Enregistrer(fibarotemps & reftemps) {
	list<fibarotemp>::iterator i;

	for (i = temps.begin(); i != temps.end(); ++i) {

		if (i->GetTemp() != reftemps.GetTemp(i->GetPiece(), i->GetId())) {
			// Double enregistrement
			// Demande l'enregistrement de l'ancienne
			reftemps.Enregistrer(i->GetPiece(), i->GetId());

			// enregistre la nouvelle
			i->Enregistrer(GetDate());

		}
	}

	reftemps = *this;

	Mysqldb::Instance().DisConnect();

	return true;
}

bool fibarotemps::VaEnregistrer(fibarotemps reftemps) {
	list<fibarotemp>::iterator i;

	for (i = temps.begin(); i != temps.end(); ++i) {

		if (i->GetTemp() != reftemps.GetTemp(i->GetPiece(), i->GetId())) {
			return true;
		}
	}

	return false;
}

bool fibarotemps::Enregistrer(int piece, int id) {
	fibarotemp valeur = GetFibaroTemp(piece, id);
	if (!valeur.IsNull()) {
		if (!valeur.EstEnregistrer()) {
			return valeur.Enregistrer(GetDate());
		}
	}
	return false;
}

fibarotemps::fibarotemps() {
	// Constructeur

	// On garde l'heure de debut de reception
	time(&datetime);
}

bool fibarotemps::AjoutTemp(int piece, int id, string nom, double temp) {
	if (!TempExiste(piece, id)) {
		struct fibarotemp ftemp;

		ftemp.Set(piece, id, nom, temp);

		log(LOG_INFO, "Enregistrement de la t°: %s", ftemp.ToString().c_str());

		temps.push_back(ftemp);
		return true;
	}

	return false;
}

bool fibarotemps::EstRenseigner() {
	return (temps.size() > 0);
}

bool fibarotemps::TempExiste(int piece, int id) {
	fibarotemp valeur = GetFibaroTemp(piece, id);
	if (!valeur.IsNull()) {
		return true;
	}
	return false;
}

double fibarotemps::GetTemp(int piece, int id) {

	fibarotemp valeur = GetFibaroTemp(piece, id);

	if (!valeur.IsNull()) {
		return valeur.GetTemp();
	}
	return -1000; // N'existe pas, force le rafraichissement
}

bool fibarotemps::fibarotemp::Enregistrer(string DateEnr) {

	if (!IsNull()) {
		if (ModeVisu) {

			log(LOG_ERR, "Sauvegarde de la T°: %s", ToString().c_str());

			return true;

		} else {
			if (!estEnregistrer) {

				string query;

				query
						= format(
								"INSERT INTO %s (datecapture, piece, id, nom, temp) VALUES (%s, %s, %s, %s, %s);",
								Mysql_TABLE.c_str(),
								prepareSqlTxtParam(DateEnr).c_str(),
								prepareSqlTxtParam(m_piece).c_str(),
								prepareSqlTxtParam(m_id).c_str(),
								prepareSqlTxtParam(m_nom.c_str()).c_str(),
								prepareSqlTxtParam(m_temp).c_str());

				bool res = Mysqldb::Instance().Exec(query);

				estEnregistrer = res;
									return res;

			}
		}
	}

	return false;

}

fibarotemps::fibarotemp fibarotemps::GetFibaroTemp(int piece, int id) {
	list<fibarotemp>::iterator i;

	for (i = temps.begin(); i != temps.end(); ++i) {
		if (i->GetPiece() == piece && i->GetId() == id) {
			return *i;
		}
	}

	fibarotemp tempNull;

	return tempNull;
}

fibarotemps::fibarotemp::fibarotemp() {
	isNull = true;
}

void fibarotemps::fibarotemp::Set(int piece, int id, string nom, double temp) {
	m_piece = piece;
	m_id = id;
	m_nom = nom;
	m_temp = temp;
	isNull = false;
}

int fibarotemps::fibarotemp::GetPiece() {
	return m_piece;
}
int fibarotemps::fibarotemp::GetId() {
	return m_id;
}
string fibarotemps::fibarotemp::GetNom() {
	return m_nom;
}
double fibarotemps::fibarotemp::GetTemp() {
	return m_temp;

}

bool fibarotemps::fibarotemp::EstEnregistrer() {
	return estEnregistrer;
}

bool fibarotemps::fibarotemp::IsNull() {
	return isNull;
}

string fibarotemps::fibarotemp::ToString() {
	return format("%i/%i: %s= %f", m_piece, m_id,
			m_nom.c_str(), m_temp);

}

