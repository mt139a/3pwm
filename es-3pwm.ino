


#include <WiFi.h>
#include <HardwareSerial.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <RTClib.h>

// Set the SSID and password of your local Wi-Fi network
const char* ssid = "moj_wifi";
const char* password = "moje_heslo";

// Create an instance of the web server
AsyncWebServer server(80);

// Set the serial port for communicating with the remote device
HardwareSerial Serial3(2);

// Set the parameters for serial communication with the remote device
const int baudRate = 9600;
const byte dataBits = 8;
const byte stopBits = 1;
const byte parity = SERIAL_8N1;
const byte flowControl = false;

// Set the initial frequency and duty cycle values
int freqValue = 101;
int duty1Value = 50;
int duty2Value = 50;
int duty3Value = 50;

// Create an instance of the RTC DS3231
RTC_DS3231 rtc;

// Function to send a command to the remote device via serial communication
void sendCommand(String command) {
  Serial3.println(command);
}

// Function to read data from the remote device via serial communication
String readData() {
  String data = "";
  while (Serial3.available() > 0) {
    char ch = Serial3.read();
    if (ch != '\n' && ch != '\r') {
      data += ch;
    }
  }
  return data;
}

// Function to parse the frequency value from the received data string
int parseFreq(String data) {
  int freq = 0;
  int index = data.indexOf("F");
  if (index >= 0) {
    freq = data.substring(index + 1).toInt();
  }
  return freq;
}

// Function to parse the duty cycle values from the received data string
void parseDuty(String data) {
  int index = data.indexOf("D1:");
  if (index >= 0) {
    duty1Value = data.substring(index + 3, index + 6).toInt();
  }
  index = data.indexOf("D2:");
  if (index >= 0) {
    duty2Value = data.substring(index + 3, index + 6).toInt();
  }
  index = data.indexOf("D3:");
  if (index >= 0) {
    duty3Value = data.substring(index + 3, index + 6).toInt();
  }
}

// Function to update the frequency value on the remote device
void updateFreq() {
  String freqCommand = "F" + String(freqValue);
  sendCommand(freqCommand);
  String response = readData();
  if (response == "DOWN") {
    Serial.println("Frequency updated successfully");
  } else {
    Serial.println("Failed to update frequency");
  }
}

// Function to update the duty cycle values on the remote device
void updateDuty() {
  String duty1Command = "D1:" + String(duty1Value);
  sendCommand(duty1Command);
  String response = readData();
  if (response == "DOWN") {
    Serial.println("Duty cycle 1 updated successfully");
  } else {
    Serial.println("Failed to update duty cycle 1");
  }
  
  String duty2Command = "D2:" + String(duty2Value);
  sendCommand(duty

2Command);
response = readData();
if (response == "DOWN") {
Serial.println("Duty cycle 2 updated successfully");
} else {
Serial.println("Failed to update duty cycle 2");
}

String duty3Command = "D3:" + String(duty3Value);
sendCommand(duty3Command);
response = readData();
if (response == "DOWN") {
Serial.println("Duty cycle 3 updated successfully");
} else {
Serial.println("Failed to update duty cycle 3");
}
}

void setup() {
// Start serial communication with the computer
Serial.begin(9600);

// Connect to Wi-Fi network
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(1000);
Serial.println("Connecting to Wi-Fi...");
}
Serial.println("Connected to Wi-Fi");

// Configure serial communication with the remote device
Serial3.begin(baudRate, dataBits, parity, stopBits, flowControl);

// Initialize the RTC DS3231
if (! rtc.begin()) {
Serial.println("Error: RTC DS3231 not found");
while (1);
}
if (rtc.lostPower()) {
rtc.adjust(DateTime(F(DATE), F(TIME)));
}

// Configure the web server to handle HTTP requests
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
// Generate the HTML code for the web page
String html = "<html><body><h1>Remote Control</h1>";
html += "<h2>Frequency:</h2>";
html += "<form method='get' action='/freq/'>";
html += "<input type='number' name='freq' min='100' max='1000' value='" + String(freqValue) + "'><br><br>";
html += "<input type='submit' value='Update frequency'></form><br><br>";
html += "<h2>Duty cycles:</h2>";
html += "<form method='get' action='/duty/'>";
html += "Duty cycle 1: <input type='number' name='duty1' min='0' max='100' value='" + String(duty1Value) + "'><br><br>";
html += "Duty cycle 2: <input type='number' name='duty2' min='0' max='100' value='" + String(duty2Value) + "'><br><br>";
html += "Duty cycle 3: <input type='number' name='duty3' min='0' max='100' value='" + String(duty3Value) + "'><br><br>";
html += "<input type='submit' value='Update duty cycles'></form><br><br>";
html += "<h2>Date and Time:</h2>";
DateTime now = rtc.now();
html += "<p>" + String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()) + "</p>";
html += "<p>" + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + "</p>";
html += "</body></html>";
// Send the HTML code to the client
request->send(200, "text/html", html);
});

// Handle HTTP GET requests to update the frequency value
server.on("/freq/", HTTP_GET, [](AsyncWebServerRequest *request){
if (request->hasParam("freq")) {
// Update the frequency value and send the

// Function to handle the frequency update request
void handleFreqUpdate(AsyncWebServerRequest *request) {
  // Get the frequency value from the request
  if (request->hasParam("freq")) {
    AsyncWebParameter* freqParam = request->getParam("freq");
    freqValue = freqParam->value().toInt();
  }
  // Update the frequency value on the remote device
  updateFreq();
  // Redirect the user back to the home page
  request->redirect("/");
}

// Function to handle the duty cycle update request
void handleDutyUpdate(AsyncWebServerRequest *request) {
  // Get the duty cycle values from the request
  if (request->hasParam("duty1")) {
    AsyncWebParameter* duty1Param = request->getParam("duty1");
    duty1Value = duty1Param->value().toInt();
  }
  if (request->hasParam("duty2")) {
    AsyncWebParameter* duty2Param = request->getParam("duty2");
    duty2Value = duty2Param->value().toInt();
  }
  if (request->hasParam("duty3")) {
    AsyncWebParameter* duty3Param = request->getParam("duty3");
    duty3Value = duty3Param->value().toInt();
  }
  // Update the duty cycle values on the remote device
  updateDuty();
  // Redirect the user back to the home page
  request->redirect("/");
}

void setup() {
  // Start serial communication with the computer
  Serial.begin(9600);

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi");

  // Configure serial communication with the remote device
  Serial3.begin(baudRate, dataBits, parity, stopBits, flowControl);

  // Initialize the RTC DS3231
  if (! rtc.begin()) {
    Serial.println("Error: RTC DS3231 not found");
    while (1);
  }
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(DATE), F(TIME)));
  }

  // Configure the web server to handle HTTP requests
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    // Generate the HTML code for the web page
    String html = "<html><body><h1>Remote Control</h1>";
    html += "<h2>Frequency:</h2>";
    html += "<form method='get' action='/freq/'>";
    html += "<input type='number' name='freq' min='100' max='1000' value='" + String(freqValue) + "'><br><br>";
    html += "<input type='submit' value='Update frequency'></form><br><br>";
    html += "<h2>Duty cycles:</h2>";
    html += "<form method='get' action='/duty


// Function to handle requests to update the duty cycle values
void handleDuty(AsyncWebServerRequest *request) {
// Get the duty cycle values from the request parameters
if (request->hasParam("duty1") && request->hasParam("duty2") && request->hasParam("duty3")) {
int duty1 = request->getParam("duty1")->value().toInt();
int duty2 = request->getParam("duty2")->value().toInt();
int duty3 = request->getParam("duty3")->value().toInt();

// Update the duty cycle values
duty1Value = duty1;
duty2Value = duty2;
duty3Value = duty3;
updateDuty();

// Send a response to the client
request->send(200, "text/plain", "Duty cycles updated successfully");

} else {
request->send(400, "text/plain", "Bad request");
}
}

void loop() {
// Handle any incoming HTTP requests
server.handleClient();

// Check if the frequency value has been updated
int newFreq = parseFreq(readData());
if (newFreq != 0 && newFreq != freqValue) {
freqValue = newFreq;
updateFreq();
}

// Check if the duty cycle values have been updated
String newData = readData();
if (newData != "" && newData != "DOWN") {
parseDuty(newData);
}

// Get the current date and time from the RTC DS3231
DateTime now = rtc.now();
String dateTime = now.toString("yyyy/MM/dd HH:mm:ss");

// Print the date and time to the serial monitor
Serial.println(dateTime);
}