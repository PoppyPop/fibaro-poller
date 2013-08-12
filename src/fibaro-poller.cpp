//============================================================================
// Name        : fibaro-poller.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <list>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <termios.h>
#include <sys/fcntl.h>
#include <getopt.h>
#include "global.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "configuration/configuration.h"
#include "json/Jzon.h"
#include "curl/curl.h"
#include "fibaro-temp.h"

using namespace std;

string pidfile;

// Gestion des erreurs
int nb_essai_max = 3;

void daemonize(void) {
	pid_t pid;

	/* Clone ourselves to make a child */
	pid = fork();

	/* If the pid is less than zero,
	 something went wrong when forking */
	if (pid < 0) {
		exit(EXIT_FAILURE);
	}

	/* If the pid we got back was greater
	 than zero, then the clone was
	 successful and we are the parent. */
	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	/* If execution reaches this point we are the child */

	/* Set the umask to zero */
	umask(0);

	pid_t sid;

	/* Try to create our own process group */
	sid = setsid();
	if (sid < 0) {
		log(LOG_ERR, "Could not create process group\n");
		exit(EXIT_FAILURE);
	}

	/* Change the current working directory */
	if ((chdir("/")) < 0) {
		log(LOG_ERR, "Could not change working directory to /\n");
		exit(EXIT_FAILURE);
	}

	/* Write the PID */
	ofstream myfile;

	myfile.open(pidfile.c_str(), ios::trunc);

	if (myfile.good()) {

		if (myfile.is_open()) {
			myfile << sid << endl;
			myfile.close();
		} else {
			log(LOG_ERR, "Unable to write PID File to '%s'", pidfile.c_str());
			exit(EXIT_FAILURE);
		}
	}

	/* Here we go */
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
		void *userp) {
	((std::string*) userp)->append((char*) contents, size * nmemb);
	return size * nmemb;
}

bool callFibaro(string fibServer, string fibUser, string fibPwd, string &datas) {
	CURL* curl; //our curl object
	string readBuffer;

	curl_global_init(CURL_GLOBAL_ALL); //pretty obvious
	curl = curl_easy_init();

	string url = format("http://%s:%s@%s/api/devices", fibUser.c_str(),
			fibPwd.c_str(), fibServer.c_str());

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

	int retCurl = curl_easy_perform(curl);
	long http_code = 0;

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

	curl_easy_cleanup(curl);
	curl_global_cleanup();

	if (retCurl == CURLE_OK && http_code == 200) {
		// On supprime les crochets
		readBuffer.replace(readBuffer.find_first_of("["), 1, "{");
		readBuffer.replace(readBuffer.find_last_of("]"), 1, "}");

		log(LOG_INFO, "Message RAW: %s", readBuffer.c_str());

		datas = readBuffer;
		return true;
	} else if (retCurl == CURLE_OK) {
		log(LOG_ERR, "Erreur de lecture avec le code HTTP %i", http_code);
	}

	return false;
}

int main(int argc, char* argv[]) {

	int c;
	int longIndex;
	unsigned int elapse = 30;
	unsigned int nbCycle = 10;
	unsigned int CycleCounter = 0;
	string fibaroServer = "192.168.0.1";
	string fibaroUser = "admin";
	string fibaroPassword = "admin";
	bool optimisationBdd = true;
	bool withWeather = false;

	ifstream inFile("/etc/conf.d/fibaro-poller");

	if (inFile.good()) {
		Configuration config;
		config.Load(inFile);

		config.Get("fibaro-server", fibaroServer);
		config.Get("fibaro-user", fibaroUser);
		config.Get("fibaro-pwd", fibaroPassword);
		config.Get("mysql-host", Mysql_HOST);
		config.Get("mysql-db", Mysql_DB);
		config.Get("mysql-table", Mysql_TABLE);
		config.Get("mysql-user", Mysql_LOGIN);
		config.Get("mysql-pwd", Mysql_PWD);
		config.Get("elapse", elapse);
		config.Get("pidfile", pidfile);
		config.Get("nombre-cycle", nbCycle);
		config.Get("with-weather", withWeather);
	}

	static const struct option longOpts[] = { { "fibaro-server",
			required_argument, NULL, 's' }, { "fibaro-user", required_argument,
			NULL, 'u' }, { "fibaro-pwd", required_argument, NULL, 'U' }, {
			"help", no_argument, NULL, 'h' }, { NULL, no_argument, NULL, 0 }, {
			"mysql-host", required_argument, NULL, 'H' }, { "mysql-db",
			required_argument, NULL, 'D' }, { "mysql-table", required_argument,
			NULL, 'T' }, { "mysql-user", required_argument, NULL, 'L' }, {
			"mysql-pwd", required_argument, NULL, 'P' }, { "elapse",
			required_argument, NULL, 'E' }, { "pidfile", required_argument,
			NULL, 'p' }, { "visualisation", no_argument, NULL, 'v' }, {
			"nombre-cycle", required_argument, NULL, 'n' }, {
			"no-bdd-optimisation", no_argument, NULL, 'o' }, { "with-weather",
			no_argument, NULL, 'w' } };

	// Traitement des paramètres
	while ((c = getopt_long(argc, argv, "dXs:hYH:D:T:L:P:E:p:vn:ozu:U:w",
			longOpts, &longIndex)) != -1)
		switch (c) {
		case 'X':
			ModeDebug = true;
			break;
		case 'd':
			ModeDeamon = true;
			break;
		case 's':
			fibaroServer = string(optarg);
			break;
		case 'u':
			fibaroUser = string(optarg);
			break;
		case 'U':
			fibaroPassword = string(optarg);
			break;
		case 'H':
			Mysql_HOST = string(optarg);
			break;
		case 'D':
			Mysql_DB = string(optarg);
			break;
		case 'T':
			Mysql_TABLE = string(optarg);
			break;
		case 'L':
			Mysql_LOGIN = string(optarg);
			break;
		case 'P':
			Mysql_PWD = string(optarg);
			break;
		case 'Y':
			ModeTest = true;
			break;
		case 'E':
			elapse = atoi(optarg);
			break;
		case 'p':
			pidfile = optarg;
			break;
		case 'v':
			ModeVisu = true;
			break;
		case 'n':
			nbCycle = atoi(optarg);
			break;
		case 'o':
			optimisationBdd = false;
			break;
		case 'z':
			DryRun = true;
			break;
		case 'w':
			withWeather = true;
			break;
		case 'h':

			printf("--help \n");
			printf("-h \t\t: Affiche l'aide \n");
			printf("-d \t\t: Mode daemon \n");
			printf("-X \t\t: Mode Debug\n");

			printf("--fibaro-server {ip ou nom dns}\n");
			printf("-s {ip ou nom dns} \t: IP or dns of home center 2\n");

			printf("--fibaro-user {username}\n");
			printf("-u {username} \t: Username of home center 2\n");

			printf("--fibaro-pwd {password}\n");
			printf("-U {password} \t: Password of home center 2\n");

			printf("--pidfile {PIDFile}\n");
			printf("-p {PIDFile} \t: Fichier contenant le PID\n");

			printf("--mysql-host {Mysql_HOST}\n");
			printf("-H {Mysql_HOST} \t: Hote Mysql\n");

			printf("--mysql-db {Mysql_DB}\n");
			printf("-D {Mysql_DB} \t: Base de données Mysql\n");

			printf("--mysql-table {Mysql_TABLE}\n");
			printf("-T {Mysql_TABLE} \t: Table Mysql\n");

			printf("--mysql-user {Mysql_LOGIN}\n");
			printf("-L {Mysql_LOGIN} \t: Login Mysql\n");

			printf("--mysql-pwd {Mysql_PWD}\n");
			printf("-P {Mysql_PWD} \t: Mot de passe Mysql\n");

			printf("--elapse {Delai en secondes}\n");
			printf(
					"-E {Delai en secondes} \t: Modifie le delai en secondes entre chaque lecture en mode daemon. (Defaut: 10)\n");

			printf("--visualisation\n");
			printf("-v \t: Mode visualisation (Test de la ligne teleinfo)\n");

			printf("--with-weather\n");
			printf("-w \t: Read weather as an temperature sensor.\n");

			printf("--nombre-cycle {int}\n");
			printf(
					"-n {int} \t: Nombre de cycle maximum sans modification en mode daemon. (Defaut: 10)\n");

			printf("--no-bdd-optimisation\n");
			printf(
					"-o \t: Desactive l'optimisation des trames en bdd (Ne pas enregistrer 2 trames identiques consécutives)\n");

			exit(0);
		case '?':

			switch (optopt) {
			case 's':
			case 'H':
			case 'D':
			case 'T':
			case 'L':
			case 'P':
			case 'n':
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				return 1;
			default:
				if (isprint(optopt)) {
					fprintf(stderr, "Unknown option `-%c'.\n", optopt);
				} else {
					fprintf(stderr, "Unknown option character `\\x%x'.\n",
							optopt);
				}
				return 1;
			}

		default:
			abort();
		}

	log(LOG_INFO, "ModeDebug : %s", (ModeDebug) ? "Oui" : "Non");
	log(LOG_INFO, "ModeDeamon : %s", (ModeDeamon) ? "Oui" : "Non");
	log(LOG_INFO, "fibaroServer : %s", fibaroServer.c_str());
	log(LOG_INFO, "elapse : %i", elapse);
	log(LOG_INFO, "pidfile : %s", pidfile.c_str());
	log(LOG_INFO, "Weather : %s", (withWeather) ? "Oui" : "Non");

	if (ModeTest) {
		cout << "No Test Mode" << endl;
		return 0;
	}

	// Mode fork
	if (ModeDeamon) {
		daemonize();
	}

	bool erreur_lecture;
	fibarotemps ancienneTemp;

	do {
		int nb_essai = 1;

		// Coeur metier
		do {
			erreur_lecture = true;

			string data;

			erreur_lecture = !callFibaro(fibaroServer, fibaroUser,
					fibaroPassword, data);

			if (!erreur_lecture) {

				Jzon::Object rootNode;
				Jzon::Parser parser(rootNode, data);

				if (!parser.Parse()) {
					log(LOG_DEBUG, "Erreur de parsing JSON: %s",
							parser.GetError().c_str());
					nb_essai++;
				} else {

					fibarotemps temps;
					try {
						for (Jzon::Object::iterator i = rootNode.begin(); i
								!= rootNode.end(); ++i) {

							std::string name = (*i).first;
							Jzon::Node &node = (*i).second;

							if (node.IsObject()) {
								Jzon::Object nodeObj = node.AsObject();

								if (nodeObj.Get("type").ToString()
										== "temperature_sensor") {

									Jzon::Object propObj = nodeObj.Get(
											"properties").AsObject();

									temps.AjoutTemp(
											nodeObj.Get("roomID").ToInt(),
											nodeObj.Get("id").ToInt(),
											nodeObj.Get("name").ToString(),
											atof(
													propObj.Get("value").ToString().c_str()));
								} else if ((nodeObj.Get("type").ToString()
										== "weather") && withWeather) {
									Jzon::Object propObj = nodeObj.Get(
											"properties").AsObject();

									temps.AjoutTemp(
											nodeObj.Get("roomID").ToInt(),
											nodeObj.Get("id").ToInt(),
											nodeObj.Get("name").ToString(),
											atof(
													propObj.Get("Temperature").ToString().c_str()));
								}
							}
						}

						erreur_lecture = false;

						if (!optimisationBdd || temps.VaEnregistrer(
								ancienneTemp) || (CycleCounter >= nbCycle)) {

							temps.Enregistrer(ancienneTemp);

							CycleCounter = 0;
						} else {
							CycleCounter++;
							log(LOG_INFO, "Pas de changement de température.");
						}

					} catch (exception exp) {
						log(LOG_ERR,
								"Erreur de lecture des informations de température.");
						nb_essai++;
					}
				}
			} else {
				log(LOG_ERR,
						"Erreur de lecture des informations de température. (Erreur Curl)");
				nb_essai++;
			}
		} while ((erreur_lecture) && (nb_essai <= nb_essai_max));

		if (ModeDeamon) {
			sleep(elapse);
		}

	} while (ModeDeamon);

	return 0;
}
