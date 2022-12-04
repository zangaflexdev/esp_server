/*
 * Ce programme permet a un ESP8266,a partir d'un point d'acces, 
 * de se connecter et de deployer un serveur.
 * Suite aux requetes des utilisateurs(a partir d'un smartphone ou PC, ...),
 * de faire un tache precis qui peut etre allumer ou eteindre une lampe
 */
 
 // Inclure la bibliotheque permetant d'utiliser la fonction WiFi de la carte
#include <ESP8266WiFi.h>

// Les references d'un point d'acces auquel la carte doit se connecter
const char* SSID = "NOM_DU_WIFI"; 
const char* PASSWORD = "MOT_DE_PASSE_DU_WIFI"; 

// On precise le port sur lequel le serveur sera deployer ici le port 80
WiFiServer server(80);

// notre lampe sur le pin GPIO 4
// correspond a D2 sur l'ESP8266
const int lampe = 4;

void setup() {
  // Lancer une communication serie (avec la machine) sur la frequence 115200
  Serial.begin(115200); 
  // configuration du GPIO 4 comme sortie
  pinMode(lampe, OUTPUT);
  //Eteindre la lampe au depart
  digitalWrite(lampe, LOW);

  Serial.print("Connecting to ");
  Serial.println(SSID);
  // Lancer la recherche et la connexion au wifi
  WiFi.begin(SSID, PASSWORD);
  // patienter le temps que le wifi se connecte
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Afficher l'address IP local attribuer a l'ESP8266 
  Serial.println("");
  Serial.println("WiFi connected at IP address:");
  Serial.println(WiFi.localIP());

  // Lancer le serveur
  server.begin();
}

// Main loop
void loop(){
  // voir l'etat du serveur
  WiFiClient client = server.available();   
  
  // Ne rien faire si le serveur n'est pas en marche
  if (!client) {
     return;
  }
  
  // Attendre un client (une page qui est demandee ou un boutton qui est appuye)
  while(!client.available()){}

  // Recuperer la requete (La demande de l'appareil)
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  int value = LOW; // Par defaut on suppose que la lampe est eteinte
  // Si dans sa requete on retouve /LED=ON alors on allume la lampe 
  if (request.indexOf("/LED=ON") != -1) 
  {
    digitalWrite(lampe, HIGH);
    value = HIGH;
  } 
  // Si dans sa requete on retouve /LED=OFF alors on allume la lampe
  if (request.indexOf("/LED=OFF") != -1)
  {
    digitalWrite(lampe, LOW);
    value = LOW;
  }
  
  // Renvoyer la page au client (Afficher la page html dans le navigateur)
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); 

  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.print("GPIO status: "); 

  if(value == HIGH) {
    client.print("ON");  
  } else {
    client.print("OFF");
  }

  client.println("<br><br>");
  client.println("Changer manuellement l'etat de la lampe");
  client.println("<br><br>");
  client.println("Turn <a href=\"/LED=ON\">ON</a><br>");
  client.println("Turn <a href=\"/LED=OFF\">OFF</a><br>");
  client.println("</html>");

  Serial.println("");
  
}
