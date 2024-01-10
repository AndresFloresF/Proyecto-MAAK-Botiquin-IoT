#include <Preferences.h>
Preferences preferences;
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
int tiempoMsg = 1000;   //tiempo medio entre escaneo de mensajes
long ultimoEscaneo;     //la última vez que se realizó el escaneo de mensajes
void manejarMensajes(int nuevoMensajes);
const char* Calibracion1="Calibracion1";
const char* Calibracion2="Calibracion2";
/*#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
// Insert Firebase project API Key
#define API_KEY "REPLACE_WITH_YOUR_FIREBASE_PROJECT_API_KEY"
// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "REPLACE_WITH_YOUR_FIREBASE_DATABASE_URL"
//Define Firebase Data object
/*FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
*/
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;


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
    bot.sendMessage(idChat, "/Bienvenida Botiquín Inteligente #xxx\n NombreRegistradoPorUsuarioParaBotiquin EN LINEA\n", "");//Enviamos un mensaje a telegram para informar que el sistema está listo
  }
}

void loop() {
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
  String CadenaDatos= ""; 
  CadenaDatos+= "Cantidad Porcentual Objeto 1 = " 
  CadenaDatos+= SensorPeso(fsrpin,Calibracion1);
  CadenaDatos= "\nCantidad Porcentual Objeto 2 = "; 
  CadenaDatos+= SensorPeso(fsrpin2,Calibracion2);
  bool DetectorObjeto3 = !digitalRead(Switch1);
  bool DetectorObjeto4 = !digitalRead(Switch2);
  CadenaDatos += "\nDetección Objeto 3 = ";
  if(DetectorObjeto3)
    CadenaDatos+="Sí\n";
  else
    CadenaDatos+="No\n";
  CadenaDatos+= "Detección Objeto 4 = ";
  if(DetectorObjeto4)
    CadenaDatos+="Sí\n";
  else
    CadenaDatos+="No\n";
  CadenaDatos+="Información enviada a tu portal Web\n";
  bot.sendMessage(idChat,CadenaDatos, "");
}

String SensorPeso(int PinSensorPeso,const char* Calibracion){ 
  preferences.begin("CalSensorPeso", true);
  short UltimaCalibracion = preferences.getShort(Calibracion, 10);
  preferences.end();
  short LecturaAnt = analogRead(PinSensorPeso);
  short LecturaActual=0;
  byte Intentos=0;
  byte Nmedicion=0;
  short PromLecturas=0;
  String CadenaPeso= "No data";
  while (Nmedicion<10){
    LecturaActual = analogRead(PinSensorPeso);
    if(((LecturaAnt+200)>LecturaActual) && (LecturaActual>LecturaAnt-200)){
       PromLecturas+=LecturaActual;
       Nmedicion++;
    }
    else{
       Intentos++;
       if(Intentos>10)
         LecturaAnt = analogRead(PinSensorPeso);
         Intentos=0;
    }
  }
  LecturaActual=PromLecturas/Nmedicion;
  if (LecturaActual>UltimaCalibracion){
      preferences.begin("CalSensorPeso", false);
      preferences.putShort(Calibracion, LecturaActual);
      UltimaCalibracion=preferences.getShort(Calibracion, UltimaCalibracion);
      preferences.end();
  }
  LecturaActual=(LecturaActual*100)/UltimaCalibracion;
  CadenaPeso=String(LecturaActual)+"%";
  return CadenaPeso;
}

void ReiniciaSensoresPeso(){
  preferences.begin("CalSensorPeso", false);
  preferences.putShort(Calibracion1, 10);
  preferences.putShort(Calibracion2, 10);
  preferences.end();
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
      ObteniendoDatos();
    }
    if (text == "/ReiniciaSensoresPeso@DetectoresBotInt_bot") {
      ReiniciaSensoresPeso();
      bot.sendMessage(idChat, "Sensores reiniciados ;)", "");
    }
    if (text == "/Bienvenida@DetectoresBotInt_bot")//Nos muestra la ayuda en telegram
    {
      String Bienvenida = "Botiquín Inteligente #xxx, NombreRegistradoPorUsuarioParaBotiquin, en línea\n";
      Bienvenida += "Da click a continuación: \n";
      Bienvenida += "/ActualizaDatosBotiquin \n";
      Bienvenida += "Actualiza los datos en tu portal y recibelos via mensaje \n\n";
      Bienvenida += "/ReiniciaSensoresPeso \n";
      Bienvenida += "Reinicia el Peso Maximo de tus sensores para un nuevo producto \n\n";
      bot.sendMessage(idChat, Bienvenida, "");
    }
  }
}
