#include <Preferences.h>
Preferences preferences;
#include "Arduino.h"
#include "WiFi.h"
#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <Wire.h>
#include "driver/rtc_io.h"
#include <LittleFS.h>
#include <FS.h>
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include <addons/TokenHelper.h>
//Replace with your network credentials
const char* ssid = "Tpablodavid";
const char* password = "Pablodavid15041971";

// Insert Firebase project API Key
#define API_KEY "AIzaSyAN-urMx6RGMWMh7bClbhsDb7N_QQkoBp8"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "andresdavid_2000@hotmail.com"
#define USER_PASSWORD "Majito510"

// Insert Firebase storage bucket ID e.g bucket-name.appspot.com
#define STORAGE_BUCKET_ID "maak-samsung.appspot.com"
// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://maak-samsung-default-rtdb.firebaseio.com"

// Photo File Name to save in LittleFS
#define FILE_PHOTO_PATH "/photo.jpg"
String AlmacenamientoFoto = "";
String RutaCantidadObjeto1 = "";
String RutaCantidadObjeto2 = "";
String RutaDeteccionObjeto3 = "";
String RutaDeteccionObjeto4 = "";
const char* NFoto = "NFoto";
const char* NDato ="NDato";
//PARA CÁMARA MODELO AI THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#define flashLed 4
boolean takeNewPhoto = true;
bool flashEstado = LOW;
int estadoFlash = 0;
//Timbre
hw_timer_t *My_timer = NULL;
#define buzzer 2
bool timbre =false;
bool Puerta =true;
bool mensaje = false;
bool EnvioExitoso=false;
bool signupOK = false;
//Define Firebase Data objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig configF;

void fcsUploadCallback(FCS_UploadStatusInfo info);
void EnviaInformacion(int CantidadObjeto1,int CantidadObjeto2,String DetecObjeto3,String DetecObjeto4);
void ModificandoRutaBaseDeDatos(void);
void ModificandoRutaAlmacenFotos(void);

// Capture Photo and Save it to LittleFS
void capturePhotoSaveLittleFS( void ) {
  // Dispose first pictures because of bad quality
  camera_fb_t* fb = NULL;
  // Skip first 3 frames (increase/decrease number as needed).
  for (int i = 0; i < 4; i++) {
    fb = esp_camera_fb_get();
    esp_camera_fb_return(fb);
    fb = NULL;
  }
    
  // Take a new photo
  fb = NULL;  
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Captuta de imagen fallida, reiniciando sistema cámara\n");
    delay(1000);
    ESP.restart();
  }  
  File file = LittleFS.open(FILE_PHOTO_PATH, FILE_WRITE);

  // Insert the data in the photo file
  if (!file) {
    Serial.println("Fallo en guardar la imagen\n");
  }
  else {
    file.write(fb->buf, fb->len); // payload (image), payload length
  }
  // Close the file
  file.close();
  esp_camera_fb_return(fb);
}

void initWiFi(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a internet y base de datos\n");
  }
}

void initLittleFS(){
  if (!LittleFS.begin(true)) {
    Serial.println("Error al generar el almacenamiento temporal para imagen\n");
    Serial.println("Reiniciando sistema cámara");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("Almacenamiento temporal para imagen listo\n");
  }
}

void IRAM_ATTR AlarmaApertura(){
  timbre=true;
}

static void IRAM_ATTR Apertura(void * arg){
  Puerta=!Puerta;
}



void setup(){
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  Serial.begin(115200);
  initWiFi();
  initLittleFS();
  pinMode(buzzer, OUTPUT);
  My_timer = timerBegin(0, 80, true);
  timerAttachInterrupt(My_timer, &AlarmaApertura, true);
  timerAlarmWrite(My_timer, 120000000, true);
  pinMode(flashLed, OUTPUT);
  digitalWrite(flashLed, flashEstado);
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;  //0-63 menor número significa mayor calidad
    config.fb_count = 1;
  }
  else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;  //0-63 menor número significa mayor calidad
    config.fb_count = 1;
  }
  // inicio de la cámara
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("El inicio de la cámara falló 0x%x", err);
    Serial.println("Reiniciando sistema cámara");
    delay(1000);
    ESP.restart();
  }
  // Tamaño de cuadro desplegable para una mayor velocidad de cuadro inicial
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_CIF);  // UXGA|SXGA|XGA|SVGA|VGA|CIF|QVGA|HQVGA|QQVGA
  // En el pin 13 se coloca el pulsador 
  err = gpio_isr_handler_add(GPIO_NUM_13, &Apertura, (void *) 13);
  if (err != ESP_OK){
    Serial.printf("hubo un error 0x%x \r\n", err); 
  }
  err = gpio_set_intr_type(GPIO_NUM_13, GPIO_INTR_POSEDGE);
  if (err != ESP_OK){
    Serial.printf("hubo un error 0x%x \r\n", err);
  } 
  if(GPIO_NUM_13==true)
    Puerta=false;
  else
    Puerta=true;

  //Firebase
  // Assign the api key
  configF.api_key = API_KEY;
  /* Assign the RTDB URL (required) */
  configF.database_url = DATABASE_URL;
  //Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  //Assign the callback function for the long running token generation task
  configF.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&configF, &auth);
  Firebase.reconnectWiFi(true);
}
void loop(){
  if(Puerta==true){
    if(mensaje==false){
      Serial.println("Botiquín abierto");
      delay(2000);
      capturePhotoSaveLittleFS();
      delay(2);
      ModificandoRutaAlmacenFotos();
      if (Firebase.ready()){
        Serial.print("Iniciando subida de imagen a base de datos... ");
        if (Firebase.Storage.upload(&fbdo, STORAGE_BUCKET_ID /* Firebase Storage bucket id */, FILE_PHOTO_PATH /* path to local file */, mem_storage_type_flash /* memory storage type, mem_storage_type_flash and mem_storage_type_sd */, AlmacenamientoFoto /* path of remote file stored in the bucket */, "image/jpeg" /* mime type */,fcsUploadCallback)){
          Serial.printf("\nURL para descargar: %s\n", fbdo.downloadURL().c_str());
        }
        else{
          Serial.println(fbdo.errorReason());
        }
      }
      timerAlarmEnable(My_timer);
      mensaje=true;
    }
    if(timbre==true){
      digitalWrite(buzzer,HIGH);
      Serial.println("Alarma exceso apertura encendida");
      timbre=false;
    }
  }
  if(Puerta==false){
    if (mensaje==true){
      timerAlarmDisable(My_timer);
      Serial.println("Botiquín cerrado");
      digitalWrite(buzzer, LOW);
      mensaje=false;
    }    
  }
  if (Serial.available()) {
    String CadenaRecibida=Serial.readStringUntil('\n');
    String value;
    byte count = 0;
    short pos = 0;
    String ValoresAenviar[4];
    while (pos < CadenaRecibida.length()) {
      if (CadenaRecibida[pos] != ',') {
        value += CadenaRecibida[pos];
      } else {
        ValoresAenviar[count] = value;
        count++;
        value = "";
      }
      pos++;
    }
    if(ValoresAenviar[3]!=""){
      int CantidadObjeto1=ValoresAenviar[0].toInt();
      int CantidadObjeto2=ValoresAenviar[1].toInt();
      ModificandoRutaBaseDeDatos();
      EnviaInformacion(CantidadObjeto1,CantidadObjeto2,ValoresAenviar[2],ValoresAenviar[3]);
      if(EnvioExitoso==true){
        Serial.println("Información enviada a tu portal Web");
        EnvioExitoso=false;
      }
    }
    else{
      Serial.println("Cadena Incompleta\nVuelve a solicitar la actualización, se tuvieron los siguientes datos solamente: ");
      Serial.println(ValoresAenviar[0]);
      Serial.println(ValoresAenviar[1]);
      Serial.println(ValoresAenviar[2]);
      Serial.println(ValoresAenviar[3]);
    }
  }
}

void EnviaInformacion(int CantidadObjeto1,int CantidadObjeto2,String DetecObjeto3,String DetecObjeto4){
  if (Firebase.ready()){
    if(Firebase.RTDB.setInt(&fbdo, RutaCantidadObjeto1, CantidadObjeto1))
      EnvioExitoso=true;
    else
      EnvioExitoso=false;
    if((Firebase.RTDB.setInt(&fbdo, RutaCantidadObjeto2, CantidadObjeto2))&&EnvioExitoso==true)
      EnvioExitoso=true;
    else
      EnvioExitoso=false;
    if((Firebase.RTDB.setString(&fbdo, RutaDeteccionObjeto3, DetecObjeto3))&&EnvioExitoso==true)
      EnvioExitoso=true;
    else
      EnvioExitoso=false;
    if((Firebase.RTDB.setString(&fbdo, RutaDeteccionObjeto4, DetecObjeto4))&&EnvioExitoso==true)
      EnvioExitoso=true;
    else
      EnvioExitoso=false;
    if(EnvioExitoso==false)
      Serial.println("Error debido a: " + fbdo.errorReason());
  }
  else{
      EnvioExitoso=false;
      Serial.println("Sin comunicación a base de datos");
  }
}

void ModificandoRutaBaseDeDatos(){
  preferences.begin("AlArch", false);
  short NumeroArchivo = preferences.getShort(NDato,0);
  RutaCantidadObjeto1 = "/BotiquinN1234/CantidadObjeto1/Dato";
  RutaCantidadObjeto1+=NumeroArchivo;
  RutaCantidadObjeto2 = "/BotiquinN1234/CantidadObjeto2/Dato";
  RutaCantidadObjeto2+=NumeroArchivo;
  RutaDeteccionObjeto3 = "/BotiquinN1234/DeteccionObjeto3/Dato";
  RutaDeteccionObjeto3+=NumeroArchivo;
  RutaDeteccionObjeto4 = "/BotiquinN1234/DeteccionObjeto4/Dato";
  RutaDeteccionObjeto4+=NumeroArchivo;
  NumeroArchivo++;
  preferences.putShort(NDato, NumeroArchivo);
  preferences.end();
}

void ModificandoRutaAlmacenFotos(){
  preferences.begin("AlArch", false);
  short NumeroArchivo = preferences.getShort(NFoto,0);
  AlmacenamientoFoto = "/Botiquin#xxx/Imagen";
  AlmacenamientoFoto += NumeroArchivo;
  AlmacenamientoFoto += ".jpg";
  NumeroArchivo++;
  preferences.putShort(NFoto, NumeroArchivo);
  preferences.end();
}

// The Firebase Storage upload callback function
void fcsUploadCallback(FCS_UploadStatusInfo info){
    if (info.status == firebase_fcs_upload_status_complete)
    {
        Serial.println("Imagen enviada a tu portal web\n");
        FileMetaInfo meta = fbdo.metaData();
        Serial.printf("Nombre: %s\n", meta.name.c_str());
        Serial.printf("Tipo de Imagen: %s\n", meta.contentType.c_str());
        Serial.printf("Tamaño: %d\n", meta.size);
    }
    else if (info.status == firebase_fcs_upload_status_error){
        Serial.printf("Error al subir a base de datos, %s\n", info.errorMsg.c_str());
    }
}
