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


String plattegrond = "";  

bool receiving = false;

typedef struct 
{
    float roomTimes[MAXS_ROOMS];
    uint8_t wrongAnswerCount;
    uint8_t totalTime;
    uint8_t difficulty;
    uint8_t maxRooms;
} runData_t;
enum __attribute__((packed)) compartment_t : uint8_t
{
    NON_C,
    ONE_C,
    TWO_C
};

enum __attribute__((packed)) specialActies_t : uint8_t
{
    NON_S,
    TOUCH_SENSOR,
    TWO_S
};

typedef struct __attribute__((packed))
{
    uint8_t coordinates[2];
    char beconIp[MAX_CHAR_IN_STRING];
    char answers[MAX_ANSWERS][MAX_CHAR_IN_STRING];
    compartment_t openCompartment;
    specialActies_t specialActies;
    char naam[MAX_CHAR_IN_STRING];
} roomSettings_t;

enum __attribute__((packed)) wrongAnswerPenalty_t : uint8_t
{
    WRONG_ANSWER_MINUS_1MIN_CONTINUE,
    WRONG_ANSWER_MINUS_5MIN_CONTINUE,
    WRONG_ANSWER_MINUS_5MIN_STOP,
    WRONG_ANSWER_MINUS_15MIN_STOP,
    WRONG_ANSWER_HALF_REMAINING_STOP 
} ;

enum __attribute__((packed)) audio_t : uint8_t
{
    AUDIO_ON,    
    AUDIO_CENSORED,    
    AUDIO_OFF
} ;

typedef struct 
{
    wrongAnswerPenalty_t difficulty;
    uint16_t totalTime;
    audio_t audio;
} globalSettings_t;

bool serverRunning = false;
bool networkRunning = false;

runData_t recordings[MAXS_RECORDINGS];
roomSettings_t rooms[MAXS_ROOMS];
globalSettings_t globalSettings;

void setup() 
{
  Serial.begin(115200);

  // WiFi.mode(WIFI_AP);
  // WiFi.softAP(ssid, password);
  // WiFi.softAPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
  // myIP = WiFi.softAPIP();

  // dnsServer.start(DNS_PORT, "*", myIP);

  Serial.println("Access Point gestart");
  Serial.print("Web adres: http://");
  Serial.println(myIP);

  if(!LittleFS.begin()) { Serial.println("Fout bij LittleFS mount"); return; }
  loadFromFlash();
  beginServer();

  loadRecordingsFromFlash();
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

  server.on("/status", HTTP_GET, []() {
    String status = serverRunning ? "online" : "offline";
    server.send(200, "text/plain", status);
});

  server.on("/save", HTTP_POST, handleSave);
  server.on("/load", HTTP_GET, handleLoad);

  server.onNotFound(handleNotFound);
  Serial.println("Webserver gestart");

  
}

void handleLoad() 
{
  DynamicJsonDocument doc(20000);

  JsonArray jsonRecordings = doc.createNestedArray("Recordings");
  for (int i = 0; i < MAXS_RECORDINGS; i++) 
  {
    if(recordings[i].roomTimes[0] > 0)
    {
      bool hasData = recordings[i].maxRooms > 0;
      if(!hasData) continue;

      JsonArray r = jsonRecordings.createNestedArray();
      JsonArray times = r.createNestedArray();
      for(int t = 0; t < MAXS_TIMES; t++)
      {
          if(recordings[i].roomTimes[t] != 0)
              times.add(recordings[i].roomTimes[t]);
      }

      r.add(false);
      r.add(recordings[i].wrongAnswerCount);
      r.add(recordings[i].totalTime);
      r.add(recordings[i].difficulty);
      r.add(recordings[i].maxRooms);
    }
  }

  JsonArray jsonKamerList = doc.createNestedArray("kamerList");
  for (int i = 0; i < MAXS_ROOMS; i++) 
  {
    if (rooms[i].naam[0] != '\0') 
    {
      JsonArray k = jsonKamerList.createNestedArray();
      k.add(nullptr);

      JsonArray posArray = k.createNestedArray(); 
      if (rooms[i].coordinates[0] >= 0 && rooms[i].coordinates[1] >= 0) 
      {
        posArray.add(rooms[i].coordinates[0]);
        posArray.add(rooms[i].coordinates[1]);
      }

      JsonObject ksd = k.createNestedObject(); 
      ksd["naam-kamer"] = rooms[i].naam;
      ksd["becon-ip"] = rooms[i].beconIp;
   
      // Maak één string van alle antwoorden gescheiden door komma
      String answersCombined = "";
    for (int a = 0; a < MAX_ANSWERS; a++) 
    {
    if (rooms[i].answers[a][0] != '\0') { // niet-lege antwoorden
        if (answersCombined.length() > 0) answersCombined += ","; // voeg komma toe
        answersCombined += String(rooms[i].answers[a]);
      }
  }

      ksd["antwoord"] = answersCombined;


      ksd["open-compartment"] = rooms[i].openCompartment;
      ksd["special-acties"] = rooms[i].specialActies;
    }
  }

  JsonObject gs = doc.createNestedObject("globalSettings");
  gs["moeilijkheid"] = globalSettings.difficulty;
  gs["start-tijd"] = globalSettings.totalTime;
  gs["audio"] = globalSettings.audio;

  if (plattegrond.length() > 0) 
    doc["uploadedImageData"] = plattegrond;

  String output;
  serializeJson(doc, output);
  server.send(200, "application/json", output);
}


void setServerOnline(bool online) 
{
    if (online && !serverRunning) 
    {
        server.begin();
        serverRunning = true;
        Serial.println("Webserver gestart via UART");
    } 
    else if (!online && serverRunning) 
    {
        server.stop();
        serverRunning = false;
        Serial.println("Webserver gestopt via UART");
    }
}

void handleSave() 
{
  Serial.println("POST ontvangen!");

  if (!server.hasArg("plain")) 
  {
    server.send(400, "text/plain", "Body ontbreekt");
    return;
  }

  String body = server.arg("plain");

  DynamicJsonDocument doc(20000);
  DeserializationError error = deserializeJson(doc, body);
  if (error) 
  {
    Serial.print("JSON fout: "); Serial.println(error.c_str());
    server.send(400, "text/plain", "Fout bij JSON parsing");
    return;
  }
 if (doc.containsKey("kamerList")) 
  {
    memset(rooms, 0, sizeof(rooms));
    JsonArray kl = doc["kamerList"];
    for (int i = 0; i < kl.size() && i < MAXS_ROOMS; i++) 
    {
      JsonObject k = kl[i][2];
      if (!k.isNull()) 
      {
          strlcpy(rooms[i].naam, k["naam-kamer"] | "", MAX_CHAR_IN_STRING);
          strlcpy(rooms[i].beconIp, k["becon-ip"] | "", MAX_CHAR_IN_STRING);
          rooms[i].openCompartment = (compartment_t)k["open-compartment"].as<int>();
          rooms[i].specialActies  = (specialActies_t)k["special-acties"].as<int>();

          Serial.println(rooms[i].openCompartment);
          if (k["antwoord"].is<const char*>())  // "antwoord" is één string
          {
              String allAnswers = k["antwoord"].as<const char*>(); // bvb "1,2,3"
              int answerIndex = 0;

              int start = 0;
              int commaPos = allAnswers.indexOf(',');
              
              while (commaPos != -1 && answerIndex < MAX_ANSWERS) 
              {
                  String part = allAnswers.substring(start, commaPos);
                  part.trim();  // verwijder spaties
                  strlcpy(rooms[i].answers[answerIndex], part.c_str(), MAX_CHAR_IN_STRING);
                  answerIndex++;
                  start = commaPos + 1;
                  commaPos = allAnswers.indexOf(',', start);
              }

              // laatste stuk na de laatste komma
              if (answerIndex < MAX_ANSWERS) 
              {
                  String part = allAnswers.substring(start);
                  part.trim();
                  strlcpy(rooms[i].answers[answerIndex], part.c_str(), MAX_CHAR_IN_STRING);
                  answerIndex++;
              }

              // leeg de rest
              for(int a = answerIndex; a < MAX_ANSWERS; a++)
                rooms[i].answers[a][0] = '\0';
          }


      }

      if (kl[i][1].is<JsonArray>())
      {
          JsonArray pos = kl[i][1].as<JsonArray>();
          rooms[i].coordinates[0] = pos.size() > 0 ? pos[0] | 0 : -1;
          rooms[i].coordinates[1] = pos.size() > 1 ? pos[1] | 0 : -1;
      }
      else
      {
          rooms[i].coordinates[0] = -1;
          rooms[i].coordinates[1] = -1;
      }
    }
  }
  if (doc.containsKey("globalSettings")) 
  {
    JsonObject gs = doc["globalSettings"];
  globalSettings.difficulty = (wrongAnswerPenalty_t)(gs["moeilijkheid"] | WRONG_ANSWER_MINUS_5MIN_CONTINUE);
  globalSettings.totalTime = gs["start-tijd"].as<int>();
  globalSettings.audio = (audio_t)gs["audio"].as<int>();
  }

  if (doc.containsKey("uploadedImageData")) 
  {
      const char* tmp = doc["uploadedImageData"];
      if (tmp != nullptr && strlen(tmp) > 0) 
          plattegrond = String(tmp);
  }

  server.send(200, "text/plain", "Data opgeslagen!");
  Serial.println("Data opgeslagen!");

  saveToFlash();
}

void charArrayToJson(JsonArray& arr, char a[][MAX_CHAR_IN_STRING], int count) 
{
    for (int i = 0; i < count; i++) arr.add(String(a[i]));
}

void jsonToCharArray(JsonArray& arr, char a[][MAX_CHAR_IN_STRING], int count) 
{
    for (int i = 0; i < count && i < arr.size(); i++)
        strlcpy(a[i], arr[i] | "", MAX_CHAR_IN_STRING);
}

void saveToFlash() 
{
  if (!LittleFS.begin()) { Serial.println("Fout bij mounten LittleFS"); return; }

  DynamicJsonDocument doc(20000);

  // Rooms
  JsonArray jsonRooms = doc.createNestedArray("rooms");
  for (int i = 0; i < MAXS_ROOMS; i++) 
  {
    if (rooms[i].naam[0] == '\0') continue;

    JsonObject r = jsonRooms.createNestedObject();
    r["naam"] = rooms[i].naam;
    r["beconIp"] = rooms[i].beconIp;
    r["open-compartment"] = rooms[i].openCompartment;
    r["special-acties"] = rooms[i].specialActies;

    JsonArray pos = r.createNestedArray("coordinates");
    pos.add(rooms[i].coordinates[0]);
    pos.add(rooms[i].coordinates[1]);

    JsonArray antwoorden = r.createNestedArray("antwoord");
    charArrayToJson(antwoorden, rooms[i].answers, MAX_ANSWERS);
  }

  // Global settings
  JsonObject gs = doc.createNestedObject("globalSettings");
  gs["moeilijkheid"] = globalSettings.difficulty;
  gs["start-tijd"] = globalSettings.totalTime;
  gs["audio"] = globalSettings.audio;

  doc["plattegrond"] = plattegrond;

  File f = LittleFS.open("/data.json", "w");
  if (!f) { Serial.println("Fout bij openen bestand voor schrijven"); return; }
  serializeJson(doc, f);
  f.close();
  Serial.println("Data opgeslagen in flash!");
}
// Laden van flash
void loadFromFlash() {
    if (!LittleFS.begin()) { Serial.println("Fout bij mounten LittleFS"); return; }
    if (!LittleFS.exists("/data.json")) { Serial.println("Geen opgeslagen data gevonden"); return; }

    File f = LittleFS.open("/data.json", "r");
    if (!f) { Serial.println("Kon bestand niet openen"); return; }

    DynamicJsonDocument doc(20000);
    DeserializationError error = deserializeJson(doc, f);
    f.close();

    if (error) { Serial.print("Fout bij JSON lezen: "); Serial.println(error.c_str()); return; }

    // Rooms
    JsonArray jsonRooms = doc["rooms"];
    for (int i = 0; i < jsonRooms.size() && i < MAXS_ROOMS; i++) 
    {
        JsonObject r = jsonRooms[i];
        strlcpy(rooms[i].naam, r["naam"] | "", MAX_CHAR_IN_STRING);
        strlcpy(rooms[i].beconIp, r["beconIp"] | "", MAX_CHAR_IN_STRING);
        rooms[i].openCompartment = (compartment_t)r["open-compartment"].as<int>();
        rooms[i].specialActies  =  (specialActies_t)r["special-acties"].as<int>();

        JsonArray pos = r["coordinates"];
        rooms[i].coordinates[0] = pos.size() > 0 ? pos[0] | 0 : -1;
        rooms[i].coordinates[1] = pos.size() > 1 ? pos[1] | 0 : -1;

            JsonArray answersArray = r["antwoord"];
      for(int j = 0; j < MAX_ANSWERS; j++) 
      {
          if(j < answersArray.size())
              strlcpy(rooms[i].answers[j], answersArray[j] | "", MAX_CHAR_IN_STRING);
          else
              rooms[i].answers[j][0] = '\0';
      }
    }
    

    // Global settings
    JsonObject gs = doc["globalSettings"];
    globalSettings.difficulty = (wrongAnswerPenalty_t)(gs["moeilijkheid"] | WRONG_ANSWER_MINUS_5MIN_CONTINUE);
    globalSettings.totalTime = gs["start-tijd"] | 0;
    globalSettings.audio = (audio_t)(gs["audio"] | AUDIO_OFF);

    plattegrond = doc["plattegrond"] | "";

    Serial.println("Data geladen uit flash!");
}

void loop() 
{
  dnsServer.processNextRequest();
  if(serverRunning) server.handleClient();

while(Serial.available())
{
    uint8_t byteIn = Serial.read();

    // Specifieke commando's
  if(!receiving) 
  {
    if (byteIn == 0xCC) 
    {  // Alles online
        if (!networkRunning) 
        {
          WiFi.softAP(ssid, password);
          WiFi.softAPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
          dnsServer.start(DNS_PORT, "*", myIP);
          networkRunning = true;
          Serial.println("Netwerk ingeschakeld");
          }
        if (!serverRunning) 
        {
          server.begin();
          serverRunning = true;
          Serial.println("Server gestart");
        }
    } 
    else if (byteIn == 0xEE) 
    { // Alles offline
      if (serverRunning) 
      {
        server.close();
        serverRunning = false;
        Serial.println("Server gestopt");
      }
      if (networkRunning) 
      {
        dnsServer.stop();
        WiFi.softAPdisconnect(true);
        WiFi.disconnect(true);
        networkRunning = false;
        Serial.println("Netwerk uitgeschakeld");
      }
      
    }
    else sentSettingData(byteIn); // bestaande functie
  }

    // Data ontvangen voor recordings
    getRunData(byteIn);
}


}

void sentSettingData(uint8_t byteIn)
{

  if (byteIn == 0xBB) 
  {

      Serial.println(globalSettings.totalTime);
      Serial.write(0xAA);
      Serial.write((uint8_t*)&globalSettings, sizeof(globalSettings));

      for(uint8_t i = 0; i < MAXS_ROOMS; i++)
      {
        uint32_t delay= 0;
        while(delay < 10000) delay++;
        
        Serial.write(0xAB);
        Serial.write((uint8_t*)&rooms[i], sizeof(roomSettings_t));
      }
    
  }
}

void getRunData(uint8_t byteIn) 
{
  static size_t index = 0;
  static runData_t runDataBuffer;
  static uint8_t* data = (uint8_t*)&runDataBuffer; 
  size_t size = sizeof(runDataBuffer);            

 
  if (!receiving) 
  {
    if (byteIn == 0xAA) 
    { 
      Serial.println("start byte ontvangen");
      receiving = true;
      index = 0;
    }
    return;
  }

  data[index++] = byteIn; 
  if (index >= size) 
  {
    receiving = false;  
    index = 0;
    Serial.println("RunData volledig ontvangen!");
    for(int i = MAXS_RECORDINGS - 1; i > 0; i--)
    {
      recordings[i] = recordings[i-1];
    }
    recordings[0] = runDataBuffer; 
    saveRecordingsToFlash() ;

  }
  
  //Save recording to flash
}

void saveRecordingsToFlash() 
{
    if (!LittleFS.begin()) { Serial.println("Fout bij mounten LittleFS"); return; }

    DynamicJsonDocument doc(20000);
    JsonArray jsonRecordings = doc.createNestedArray("Recordings");

    for (int i = 0; i < MAXS_RECORDINGS; i++) 
    {
        bool hasData = recordings[i].maxRooms > 0; // check of deze recording geldig is
        if (!hasData) continue;

        JsonObject r = jsonRecordings.createNestedObject();
        
        JsonArray times = r.createNestedArray("roomTimes");
        for (int t = 0; t < MAXS_TIMES; t++) 
        {
            if (recordings[i].roomTimes[t] != 0)
                times.add(recordings[i].roomTimes[t]);
        }

        r["wrongAnswerCount"] = recordings[i].wrongAnswerCount;
        r["totalTime"] = recordings[i].totalTime;
        r["difficulty"] = recordings[i].difficulty;
        r["maxRooms"] = recordings[i].maxRooms;
    }

    File f = LittleFS.open("/recordings.json", "w");
    if (!f) { Serial.println("Fout bij openen bestand voor schrijven"); return; }

    serializeJson(doc, f);
    f.close();
    Serial.println("Recordings opgeslagen in flash!");
}
void loadRecordingsFromFlash()
{
    if (!LittleFS.begin()) { Serial.println("Fout bij mounten LittleFS"); return; }
    if (!LittleFS.exists("/recordings.json")) { Serial.println("Geen recordings gevonden"); return; }

    File f = LittleFS.open("/recordings.json", "r");
    if (!f) { Serial.println("Kon recordings niet openen"); return; }

    DynamicJsonDocument doc(20000);
    DeserializationError error = deserializeJson(doc, f);
    f.close();

    if (error) { Serial.print("Fout bij JSON lezen: "); Serial.println(error.c_str()); return; }

    JsonArray arr = doc["Recordings"];
    for(int i = 0; i < arr.size() && i < MAXS_RECORDINGS; i++)
    {
        JsonObject r = arr[i];
        JsonArray times = r["roomTimes"];
        for(int t = 0; t < MAXS_TIMES; t++)
            recordings[i].roomTimes[t] = t < times.size() ? times[t].as<float>() : 0;

        recordings[i].wrongAnswerCount = r["wrongAnswerCount"] | 0;
        recordings[i].totalTime = r["totalTime"] | 0;
        recordings[i].difficulty = r["difficulty"] | 0;
        recordings[i].maxRooms = r["maxRooms"] | 0;
    }

    Serial.println("Recordings geladen uit flash!");
}

void handleNotFound() 
{
    File f = LittleFS.open("/index.html", "r");
    if(!f) { server.send(500, "text/plain", "LittleFS fout"); return; }
    server.streamFile(f, "text/html");
    f.close();
}