#include "html.h"

const char* ssid = "qlab_goscie";
const char* password = "qlab2023";

WebServer server(80);

bool logSerial1 = false;

// String HTML
String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: Arial, sans-serif;
      max-width: 600px;
      margin: 0 auto;
      padding: 20px;
      background-color: #f5f5f5;
    }
    .card {
      background: white;
      padding: 20px;
      border-radius: 8px;
      box-shadow: 0 2px 4px rgba(0,0,0,0.1);
      margin-bottom: 20px;
    }
    h1 {
      color: #444;
      text-align: center;
    }
    .form-group {
      margin-bottom: 15px;
    }
    input[type="file"] {
      width: 100%;
      padding: 10px;
      border: 1px solid #ddd;
      border-radius: 4px;
    }
    input[type="submit"], .btn {
      background-color: #4CAF50;
      color: white;
      border: none;
      padding: 10px 15px;
      border-radius: 4px;
      cursor: pointer;
      text-decoration: none;
      display: inline-block;
      font-size: 16px;
    }
    input[type="submit"]:hover, .btn:hover {
      background-color: #45a049;
    }
    .checkbox-container {
      display: flex;
      align-items: center;
      margin-top: 20px;
    }
    .checkbox-container input {
      margin-right: 10px;
    }
    .status {
      margin-top: 20px;
      padding: 10px;
      border-radius: 4px;
    }
    .success {
      background-color: #dff0d8;
      color: #3c763d;
    }
    .error {
      background-color: #f2dede;
      color: #a94442;
    }
  </style>
</head>
<body>
  <div class="card">
    <h1>ESP32 Flasher</h1>
    
    <div class="form-group">
      <h3>Upload Firmware</h3>
      <form method='POST' action='/upload' enctype='multipart/form-data'>
        <input type='file' name='plik'><br><br>
        <input type='submit' value='Upload'>
      </form>
    </div>
    
    <div class="form-group">
      <h3>Flash ESP32</h3>
      <a href='/flash' class='btn'>Start Flashing</a>
    </div>
    
    <div class="checkbox-container">
      <input type='checkbox' id='logToggle'>
      <label for='logToggle'>Pokaz komunikacje slave w Serial</label>
    </div>
  </div>

  <script>
    document.getElementById('logToggle').addEventListener('change', function() {
      fetch('/togglelogs?enable=' + (this.checked ? '1' : '0'))
        .then(response => response.text())
        .then(data => {
          console.log('Logging toggled:', data);
        });
    });
  </script>
</body>
</html>
)rawliteral";

// Konfiguracja polaczenia WiFi oraz WebServera
void wifi_setup(){
    // Polacz z WiFi
    WiFi.begin(ssid, password);
    Serial.print("Łączenie z WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Połączono, IP: ");
    Serial.println(WiFi.localIP());

    // WebServer
    server.on("/", HTTP_GET, handleRoot);
    server.on("/upload", HTTP_POST, []() {
        server.send(200);
    }, upload_programm);
    //server.on("/flash", HTTP_GET, handleFlash);
    server.on("/togglelogs", HTTP_GET, handleToggleLogs);
    server.begin();
    Serial.println("Serwer HTTP uruchomiony");
    }

// Konfiguracja SPIFFSu
void spiffs_setup(){
    if (!SPIFFS.begin(true)) {
        Serial.println("Błąd montowania SPIFFS");
    return;
    }
    Serial.println("SPIFFS zamontowany");
}

// Wgrywanie programu NCL na SPIFFS testera
void upload_programm(){
HTTPUpload& upload = server.upload();
    // Rozpoczecie uploadu
    if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("Rozpoczynanie uploadu: %s\n", upload.filename.c_str());
        if (SPIFFS.exists("/" + upload.filename)) {
        SPIFFS.remove("/" + upload.filename);
        }
        // Otwarcie pliku do zapisu
        File file = SPIFFS.open("/" + upload.filename, FILE_WRITE);
        if (!file) {
        Serial.println("Błąd otwarcia pliku do zapisu");
        server.send(500, "text/plain", "Błąd zapisu pliku");
        return;
        }
        file.close();
    // Dopisywanie fragmentow pliku
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        File file = SPIFFS.open("/" + upload.filename, FILE_APPEND);
        if (file) {
        file.write(upload.buf, upload.currentSize);
        file.close();
        } else {
        Serial.println("Błąd zapisu fragmentu pliku");
        }
    // Zakonczenie zapisu pliku do SPIFFS
    } else if (upload.status == UPLOAD_FILE_END) {
        Serial.printf("Upload zakończony: %s, rozmiar: %u bajtów\n", upload.filename.c_str(), upload.totalSize);
        server.send(200, "text/plain", "Upload zakończony");
    }
}

void handleRoot() {
  server.send(200, "text/html", html);
}

// ON / OFF logowanie slavea
void handleToggleLogs() {
  if (server.hasArg("enable")) {
    logSerial1 = server.arg("enable") == "1";
    Serial.print("Logowanie slave: ");
    Serial.println(logSerial1 ? "WŁĄCZONE" : "WYŁĄCZONE");
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Brak parametru");
  }
}