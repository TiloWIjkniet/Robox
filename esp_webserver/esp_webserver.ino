#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <DNSServer.h>

const char* ssid = "Robox Admin";
const char* password = "12345678";

ESP8266WebServer server(80);


const byte DNS_PORT = 53;
DNSServer dnsServer;
IPAddress myIP;

#define MAXS_CHAR 100
#define MAXS_TIMES 15
#define MAXS_RECORDINGS 10
#define MAXS_ROOMS 20
#define MAX_CHAR_IN_STRING 50
#define MAX_ANSWERS 5

typedef struct 
{
    float roomTimes[MAXS_ROOMS];
    uint8_t wrongAnswerCount;
    uint8_t totalTime;
    uint8_t difficulty;
    uint8_t maxRooms;
}runData_t;

typedef struct __attribute__((packed))
{
    uint8_t coordinates[2];
    char beconIp[MAX_CHAR_IN_STRING];
    char answers[MAX_ANSWERS][MAX_CHAR_IN_STRING];
    uint8_t openCompartment;
    uint8_t specialActies;
} roomSettings_t;

typedef enum
{
    WRONG_ANSWER_MINUS_1MIN_CONTINUE,     // 1 minuut aftrek, spel gaat door
    WRONG_ANSWER_MINUS_5MIN_CONTINUE,     // 5 minuten aftrek, spel gaat door
    WRONG_ANSWER_MINUS_5MIN_STOP,
    WRONG_ANSWER_MINUS_15MIN_STOP,   // Tijd aftrekken (bijv. 5 min), stop als tijd 0
    WRONG_ANSWER_HALF_REMAINING_STOP 
}wrongAnswerPenalty_t;

typedef enum
{
    AUDIO_ON,    
    AUDIO_CENSORED,    
    AUDIO_OFF
}audio_t;

typedef struct 
{
    wrongAnswerPenalty_t difficulty;
    float totalTime;
    audio_t audio;

}globalSettings_t;

runData_t recordings[MAXS_RECORDINGS];
roomSettings_t rooms[MAXS_ROOMS];
globalSettings_t globalSettings;
String plattegrond = "";  




void setup() 
{
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  WiFi.softAPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
  myIP = WiFi.softAPIP();
  dnsServer.start(DNS_PORT, "*", myIP);
  Serial.println("Access Point gestart");
  Serial.print("Web adres: http://");
  Serial.println(myIP);

  if(!LittleFS.begin()) { Serial.println("Fout bij LittleFS mount"); return;}
  loadFromFlash();
  beginServer();
  
}

void beginServer()
{
   server.on("/", HTTP_GET, []()
  { 
    File f = LittleFS.open("/index.html", "r");
    server.streamFile(f, "text/html");
    f.close();
  });

  server.on("/style.css", HTTP_GET, []()
  {
    File f = LittleFS.open("/style.css", "r");
    server.streamFile(f, "text/css");
    f.close();
  });

  server.on("/script.js", HTTP_GET, []()
  {
    File f = LittleFS.open("/script.js", "r");
    server.streamFile(f, "application/javascript");
    f.close();
  });

  server.on("/save", HTTP_POST, handleSave);
  server.on("/load", HTTP_GET, handleLoad);

  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println("Webserver gestart");
}

void handleLoad() 
{
  DynamicJsonDocument doc(20000);


  JsonArray jsonRecordings = doc.createNestedArray("Recordings");
  for (int i = 0; i < MAXS_RECORDINGS; i++) 
  {
    if(recordings[i].times[0] > 0)
    {
      bool hasData = recordings[i].amountOfRooms > 0; // bijvoorbeeld
      if(!hasData) continue;

      JsonArray r = jsonRecordings.createNestedArray();
      // tijden array
      JsonArray times = r.createNestedArray();
      for(int t = 0; t < MAXS_TIMES; t++)
      {
          if(recordings[i].times[t] != 0) // alleen echte tijden
              times.add(recordings[i].times[t]);
      }

      // overige velden
      r.add(false);
      r.add(recordings[i].errors);
      r.add(recordings[i].startTime);
      r.add(recordings[i].difficulty);
      r.add(recordings[i].amountOfRooms);
    }
  }

  // Kamerlijst terugzetten
  JsonArray jsonKamerList = doc.createNestedArray("kamerList");
  for (int i = 0; i < MAXS_ROOMS; i++) 
  {
    if (rooms[i].name[0] != '\0') // check of de kamer een naam heeft
    {
      JsonArray k = jsonKamerList.createNestedArray();
      k.add(nullptr); // placeholder

      // Voeg de positie toe als array [x, y] of leeg array als niet ingesteld
      JsonArray posArray = k.createNestedArray(); // dit wordt k[1]
      if (rooms[i].pos[0] >= 0 && rooms[i].pos[1] >= 0) 
      {
        posArray.add(rooms[i].pos[0]);
        posArray.add(rooms[i].pos[1]);
      }

      // Kamerdata
      JsonObject ksd = k.createNestedObject(); // k[2]
      ksd["naam-kamer"] = rooms[i].name;
      ksd["becon-ip"] = rooms[i].beaconIp;
      ksd["antwoord"] = rooms[i].answers;
      ksd["open-compartment"] = rooms[i].compartment;
      ksd["special-acties"] = rooms[i].actie;
    }
  }

  // Global settings
  JsonObject gs = doc.createNestedObject("globalSettings");
  gs["moeilijkheid"] = globalSettings.difficulty;
  gs["start-tijd"] = globalSettings.startTime;
  gs["audio"] = globalSettings.audio;


  if (plattegrond.length() > 0) 
  {
    doc["uploadedImageData"] = plattegrond;
  } 

  String output;
  serializeJson(doc, output);
  server.send(200, "application/json", output);
}

void handleSave() 
{
  Serial.println("POST ontvangen!");  // check of de route geraakt wordt

  if (server.hasArg("plain") == false) 
  {
    server.send(400, "text/plain", "Body ontbreekt");
    return;
  }

  String body = server.arg("plain");

  DynamicJsonDocument doc(20000);
  DeserializationError error = deserializeJson(doc, body);

  if (error) 
  {
    Serial.print("JSON fout: ");
    Serial.println(error.c_str());
    server.send(400, "text/plain", "Fout bij JSON parsing");
    return;
  }

  // Kamerlijst opslaan
  if (doc.containsKey("kamerList")) 
  {
    memset(rooms, 0, sizeof(rooms));
    JsonArray kl = doc["kamerList"];
    Serial.println(kl.size());
    for (int i = 0; i < kl.size() && i < MAXS_ROOMS; i++) 
    {
      // Kamer object
      JsonObject k = kl[i][2]; // object
      if (!k.isNull()) 
      {
          strlcpy(rooms[i].name, k["naam-kamer"] | "", MAXS_CHAR);
          strlcpy(rooms[i].beaconIp, k["becon-ip"] | "", MAXS_CHAR);
          strlcpy(rooms[i].answers, k["antwoord"] | "", MAXS_CHAR);
          rooms[i].compartment = k["open-compartment"].as<int>() | 0;
          rooms[i].actie = k["special-acties"].as<int>() | 0;
      }

      // Kamer positie
      if (kl[i][1].is<JsonArray>())
      {
          JsonArray pos = kl[i][1].as<JsonArray>();
          rooms[i].pos[0] = pos.size() > 0 ? pos[0] | 0 : -1;
          rooms[i].pos[1] = pos.size() > 1 ? pos[1] | 0 : -1;
      }
      else
      {
          rooms[i].pos[0] = -1;
          rooms[i].pos[1] = -1;
      }
    }
  }

  if (doc.containsKey("globalSettings")) 
  {
    JsonObject gs = doc["globalSettings"];

    globalSettings.difficulty =gs["moeilijkheid"] | 0;

    globalSettings.startTime =gs["start-tijd"] | 0;

    globalSettings.audio =gs["audio"].as<int>() | 0;
  }

  // Plattegrond opslaan
  if (doc.containsKey("uploadedImageData")) 
  {
      const char* tmp = doc["uploadedImageData"]; // leest de string
      if (tmp != nullptr && strlen(tmp) > 0) // check op null en lege string
      {
          plattegrond = String(tmp);
      }
  }
  server.send(200, "text/plain", "Data opgeslagen!");
  Serial.println("Data opgeslagen!");
  saveToFlash();
}

void saveToFlash() {
    if (!LittleFS.begin()) {
        Serial.println("Fout bij mounten LittleFS");
        return;
    }

    DynamicJsonDocument doc(20000);

    // Rooms opslaan
    JsonArray jsonRooms = doc.createNestedArray("rooms");
    for (int i = 0; i < MAXS_ROOMS; i++) {
        JsonObject r = jsonRooms.createNestedObject();
        r["name"] = rooms[i].name;
        r["answers"] = rooms[i].answers;
        r["beaconIp"] = rooms[i].beaconIp;
        r["compartment"] = rooms[i].compartment;
        r["actie"] = rooms[i].actie;

        // Positie als array
        JsonArray pos = r.createNestedArray("pos");
        pos.add(rooms[i].pos[0]);
        pos.add(rooms[i].pos[1]);
    }

    // Global settings
    JsonObject gs = doc.createNestedObject("globalSettings");
    gs["difficulty"] = globalSettings.difficulty;
    gs["startTime"] = globalSettings.startTime;
    gs["audio"] = globalSettings.audio;

    // Plattegrond
    doc["plattegrond"] = plattegrond;

    // Opslaan naar LittleFS
    File f = LittleFS.open("/data.json", "w");
    if (!f) {
        Serial.println("Fout bij openen bestand voor schrijven");
        return;
    }

    serializeJson(doc, f);
    f.close();

    Serial.println("Data opgeslagen in flash!");
}

void loadFromFlash() {
    if (!LittleFS.begin()) {
        Serial.println("Fout bij mounten LittleFS");
        return;
    }

    if (!LittleFS.exists("/data.json")) {
        Serial.println("Geen opgeslagen data gevonden");
        return;
    }

    File f = LittleFS.open("/data.json", "r");
    if (!f) {
        Serial.println("Kon bestand niet openen");
        return;
    }

    DynamicJsonDocument doc(20000);
    DeserializationError error = deserializeJson(doc, f);
    f.close();

    if (error) {
        Serial.print("Fout bij JSON lezen: ");
        Serial.println(error.c_str());
        return;
    }

    // Rooms inlezen
    JsonArray jsonRooms = doc["rooms"];
    for (int i = 0; i < jsonRooms.size() && i < MAX_CHAR_IN_STRING; i++) {
        JsonObject r = jsonRooms[i];
        strlcpy(rooms[i].name, r["name"] | "", MAX_CHAR_IN_STRING);
        strlcpy(rooms[i].answers, r["answers"] | "", MAX_CHAR_IN_STRING);
        strlcpy(rooms[i].beaconIp, r["beaconIp"] | "", MAX_CHAR_IN_STRING);
        rooms[i].compartment = r["compartment"] | 0;
        rooms[i].actie = r["actie"] | 0;

        JsonArray pos = r["pos"];
        rooms[i].pos[0] = pos.size() > 0 ? pos[0] | 0 : -1;
        rooms[i].pos[1] = pos.size() > 1 ? pos[1] | 0 : -1;
    }

    // Global settings
    JsonObject gs = doc["globalSettings"];
    globalSettings.difficulty = gs["difficulty"] | 0;
    globalSettings.startTime = gs["startTime"] | 0;
    globalSettings.audio = gs["audio"] | 0;

    // Plattegrond
    plattegrond = doc["plattegrond"] | "";

    Serial.println("Data geladen uit flash!");
}

void loop() 
{
  dnsServer.processNextRequest();
  server.handleClient();
  getRunData();
  sentSettingData();
}
void sentSettingData()
{

  if(!Serial.available()) return;
  uint8_t cmd = Serial.read();
  if (cmd != 0xBB) return;

  Serial.write((uint8_t*)rooms, sizeof(rooms));
  String.write((uint8_t*)globalSettings, sizeof(globalSettings));
}
void getRunData()
{
  if(!Serial.available()) return;
  uint8_t cmd = Serial.read();
  if (cmd != 0xAA) return;
  
  size_t size = sizeof(runDatas);
  uint8_t* data = (uint8_t*)runDatas;

  for (size_t i = 0; i < size; i++)
  {
      while (!Serial.available());
      data[i] = Serial.read();
  }

}

     
void handleNotFound() {
    // Open index.html voor ALLE requests
    File f = LittleFS.open("/index.html", "r");
    if(!f) {
        server.send(500, "text/plain", "LittleFS fout");
        return;
    }
    server.streamFile(f, "text/html");
    f.close();
}
