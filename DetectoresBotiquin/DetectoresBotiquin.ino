#include <Preferences.h>
Preferences preferences;
preferences.begin("CalSensorPeso", false); 

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
bool Detector1;
bool Detector2;
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

/*void ObteniendoDatos(){
  String CadenaDatos= "Objeto Peso 1 = "; 
  CadenaDatos+= SensorPeso(fsrpin,1);
  String CadenaDatos= "Objeto Peso 2 = "; 
  CadenaDatos+= SensorPeso(fsrpin2,2);
  DetectorObjeto3 = !digitalRead(Switch1);
  DetectorObjeto4 = !digitalRead(Switch2);
  CadenaDatos += "Detección Objeto 3 = ";
  CadenaDatos+= DetectorObjeto3;
  CadenaDatos+="\n";
  CadenaDatos+= "Detección Objeto 4 = ";
  CadenaDatos+= DetectorObjeto4;
  CadenaDatos+="\n";
  CadenaDatos+="Información enviada a tu portal Web\n";
  bot.sendMessage(idChat, CadenaDatos, "");
  DatosAhora=false;
}*/

String SensorPeso(int PinSensorPeso,short NumeroSensorPeso){
  String ObtCal="Calibracion"+NumeroSensorPeso;
  preferences.begin("CalSensorPeso", true); 
  short UltimaCalibracion = getShort(ObtCal, 0)
  preferences.end();
  
  short LecturaAnt = analogRead(PinSensorPeso);
  short LecturaActual=0;
  byte Intentos=0;
  byte Nmedicion=0;
  short PromLecturas;
  String CadenaPeso= fsrreading;
  while Nmedicion<10:
    LecturaActual = analogRead(PinSensorPeso);
    if(((LecturaAnt+200)>LecturaActual) && (LecturaActual>LecturaAnt-200)){
       PromLecturas+=LecturaActual
       Nmedicion++;
    }
    else{
       Intentos++;
       if(Intentos>10)
         LecturaAnt = analogRead(PinSensorPeso);
    }
  LecturaActual=PromLecturas/Nmedicion;
  if (LecturaActual>UltimaCalibracion){
      preferences.begin("CalSensorPeso", false);
      preferences.putUShort(ObtCal, LecturaActual);
      preferences.end();
  }
  LecturaActual=(LecturaActual/UltimaCalibracion)*100
  CadenaPeso=LecturaActual+"%";
  return CadenaPeso;
}

/*void manejarMensajes(int nuevoMensajes){
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
    if (text == "/Bienvenida")//Nos muestra la ayuda en telegram
    {
      String Bienvenida = "Botiquín Inteligente #xxx, NombreRegistradoPorUsuarioParaBotiquin, en línea\n";
      Bienvenida += "¿Deseas actualizar tus datos del botiquin ahora??\n\n";
      Bienvenida += "Da click a continuación: \n";
      Bienvenida += "/ActualizaDatosBotiquin \n";
      Bienvenida += "Recuerda el sistema distingue entre mayuculas y minusculas \n";
      bot.sendMessage(idChat, Bienvenida, "");
    }
  }
}*/
