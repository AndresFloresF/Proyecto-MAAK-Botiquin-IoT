#define fsrpin 34
#define fsrpin2 32
#define Switch1 33
#define Switch2 25
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
const char* ssid = "Tpablodavid";
const char* password = "Pablodavid15041971";
String idChat = "-4012404846";//ID_Chat se obtiene de telegram
String token = "6935071361:AAHMK8o85_fqfNKv_0KiTFcOnIuotpF9XHg";
WiFiClientSecure clientTCP;
UniversalTelegramBot bot(token, clientTCP);
int fsrreading; //Variable to store FSR value
int fsrreading2; //Variable to store FSR value
bool Detector1;
bool Detector2;
bool DatosAhora=false;
int tiempoMsg = 1000;   //tiempo medio entre escaneo de mensajes
long ultimoEscaneo;     //la última vez que se realizó el escaneo de mensajes
void manejarMensajes(int nuevoMensajes);

void setup() {
  Serial.begin(115200);
  pinMode(Switch1, INPUT);
  pinMode(Switch2, INPUT);
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Agregar certificado raíz para api.telegram.org
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    bot.sendMessage(idChat, "Sistema de detectores Botiquín Inteligente EN LINEA :)      !!!\n Escribe /Opciones para ver las posibles acciones", "");//Enviamos un mensaje a telegram para informar que el sistema está listo
  }
}

void loop() {
  if (DatosAhora==true)
     ObteniendoDatos();
  if (millis() > ultimoEscaneo + tiempoMsg){
    int nuevoMensajes = bot.getUpdates(bot.last_message_received + 1);
    while (nuevoMensajes){
      manejarMensajes(nuevoMensajes);
      nuevoMensajes = bot.getUpdates(bot.last_message_received + 1);
    }
    ultimoEscaneo = millis();
  }   
}

void ObteniendoDatos(){
  Detector1 = !digitalRead(Switch1);
  Detector2 = !digitalRead(Switch2);
  String CadenaDatos = "Detector 1 = ";
  CadenaDatos+= Detector1;
  CadenaDatos+="\n";
  CadenaDatos+= "Detector 2 = ";
  CadenaDatos+= Detector2;
  CadenaDatos+="\n";
  fsrreading = analogRead(fsrpin);
  fsrreading2 = analogRead(fsrpin2);
  CadenaDatos+= "Objeto Peso 1 = ";
  CadenaDatos+= fsrreading;
  if (fsrreading < 10) {
    CadenaDatos+= " - Sin objeto\n";
  } else if (fsrreading < 200) {
    CadenaDatos+= " - 25%\n";
  } else if (fsrreading < 600) {
    CadenaDatos+= " - 50%\n";
  } else if (fsrreading < 900) {
    CadenaDatos+= " - 75%\n";
  } else if (fsrreading < 1200) {
    CadenaDatos+= " - 100%\n";
  } 
  else {
    CadenaDatos+= " - Objeto demasiado pesado, recalibra \n";
  }
  CadenaDatos+= "Objeto Peso 2 = ";
  CadenaDatos+= fsrreading2;
  if (fsrreading2 < 10) {
    CadenaDatos+= " - Sin objeto\n";
  } else if (fsrreading2 < 200) {
    CadenaDatos+= " - 25%\n";
  } else if (fsrreading2 < 600) {
    CadenaDatos+= " - 50%\n";
  } else if (fsrreading2 < 900) {
    CadenaDatos+= " - 75%\n";
  } else if (fsrreading2 < 1200) {
    CadenaDatos+= " - 100%\n";
  } 
  else {
    CadenaDatos+= " - Objeto demasiado pesado, recalibra\n";
  }
  bot.sendMessage(idChat, CadenaDatos, "");
  DatosAhora=false;
}

void manejarMensajes(int nuevoMensajes){
  for (int i = 0; i < nuevoMensajes; i++){
    // ID de chat del solicitante
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != idChat){
      bot.sendMessage(idChat, "Usuario no autorizado", "");
      continue;
    }   
    // Imprime el mensaje recibido
    String text = bot.messages[i].text;
    if (text == "/ActualizaDatosBotiquin@DetectoresBotInt_bot") {
      DatosAhora = true;
    }
    if (text == "/CalibrarPeso1@DetectoresBotInt_bot") {
      bot.sendMessage(idChat, "Solicitando Calibración PESO 1", "");
    }
    if (text == "/CalibrarPeso2@DetectoresBotInt_bot") {
      bot.sendMessage(idChat, "Solicitando Calibración PESO 2", "");
    }
    if (text == "/Opciones@DetectoresBotInt_bot")//Nos muestra la ayuda en telegram
    {
      String ayuda = "Bienvenido al sistema de detectores Botiquín Inteligente.\n";
      ayuda += "Estas son tus opciones.\n\n";
      ayuda += "/CalibrarPeso1 \n";
      ayuda += "/CalibrarPeso2 \n";
      ayuda += "/ActualizaDatosBotiquin \n";
      ayuda += "/Opciones: Imprime este menú \n";
      ayuda += "Recuerda el sistema distingue entre mayuculas y minusculas \n";
      bot.sendMessage(idChat, ayuda, "");
    }
  }
}
