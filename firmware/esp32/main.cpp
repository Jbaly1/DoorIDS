#include <Arduino.h>
#include <WiFi.h>

// ========= PINS ==================
constexpr uint8_t reedPin = 21;
constexpr uint8_t buzzerPin = 18;

// ========== STATES ================
bool lastReedPinState = true; 
bool debounceState = true;

// ======== WIFI =======================
const char* ssid = "YOUR SSID";
const char* password = "YOUR PASSWORD";

const char* raspberryPi = "192.168.1.3";
uint16_t portNumber = 5000;

WiFiClient client;

// ======= DOOR ==============

uint32_t doorHeldTime = 0;
bool doorHeldReported = false;

char doorOpen = 'O';
char doorClosed = 'C';
char doorHeld = 'H';

// ===== BUZZER ===========
bool buzzerActive = false;
// ==========================


struct Timer{
    const uint16_t interval;
    uint32_t lastTime;
};

Timer reed{100, 0};
Timer door{1000, 0};
Timer buzzer{1000, 0};
Timer tcpConnection{5000, 0};
Timer WiFiConnection{5000, 0};

void connectToWiFi(){
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
}

void connectToRaspberryPi(){
    client.connect(raspberryPi, portNumber);
}

void handleWiFi(uint32_t currentTime){
    static bool connecting = false;
    static uint32_t previousAttempt = 0;

    wl_status_t status = WiFi.status();

    // If already connected, reset state and leave this function
    if (status == WL_CONNECTED) {
        connecting = false;
        return;
    }

    // If not connected, decide whether to start / retry
    if (!connecting && (currentTime - previousAttempt >= WiFiConnection.interval)) {
        previousAttempt = currentTime;
        connecting = true;

        WiFi.disconnect(true);
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
    }

    // If `connecting` is true, it's calm
    // WiFi tries in the background will just keep looping
}

void handleTCP(uint32_t currentTime){
    if(!client.connected()){
        if(currentTime - tcpConnection.lastTime >= tcpConnection.interval){
            tcpConnection.lastTime = currentTime;
            client.stop();
            connectToRaspberryPi();
        }
    }
}

void buzzerOn(uint32_t currentTime){
    digitalWrite(buzzerPin, HIGH);
    buzzer.lastTime = currentTime;
    buzzerActive = true;
}

void buzzerOff(uint32_t currentTime){
    if(buzzerActive && currentTime - buzzer.lastTime >= buzzer.interval){
        digitalWrite(buzzerPin, LOW);
        buzzerActive = false;
    }
}

void isDoorHeld(uint32_t currentTime, bool &debounceState){
    if(debounceState){

        if(currentTime - door.lastTime >= door.interval){
            doorHeldTime += 1;
            door.lastTime = currentTime;
        }

        if(doorHeldTime == 20 && doorHeldReported == false){
            client.print(doorHeld);
            doorHeldReported = true;
        }
                
    }else{
        doorHeldTime = 0;
        doorHeldReported = false;
        }
}
    

void setup(){
    pinMode(reedPin, INPUT_PULLUP);
    pinMode(buzzerPin, OUTPUT);
    delay(1000);

    connectToWiFi();
    connectToRaspberryPi();
}


void loop(){
    uint32_t currentTime = millis();

    // Ensure WiFi is still up
    handleWiFi(currentTime);

    // Ensure TCP connection to Raspberry Pi
    handleTCP(currentTime);

    bool reedPinState = digitalRead(reedPin);

    if(reedPinState != lastReedPinState){
        lastReedPinState = reedPinState;
        reed.lastTime = currentTime;
    }

    if(currentTime - reed.lastTime >= reed.interval){
        if(debounceState != reedPinState){
            debounceState = reedPinState;

            if(debounceState){
                buzzerOn(currentTime);
                client.print(doorOpen);
                  
            }else{
                client.print(doorClosed);
            }
        }

        else{
            isDoorHeld(currentTime, debounceState);
        }

        
        buzzerOff(currentTime);
        
    }
    

}
