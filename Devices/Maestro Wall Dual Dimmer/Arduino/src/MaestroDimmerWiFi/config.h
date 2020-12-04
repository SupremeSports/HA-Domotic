// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- NETWORK SETUP ------------------------------------------
// ----------------------------------------------------------------------------------------------------
//You must put your own settings in here

const char* ssid                      = "yourSSID";
const char* password                  = "yourPassword";
const char* passwordWrong             = "TEST1234";

const char* mqtt_server               = "192.168.1.10";
const char* mqtt_user                 = "yourMQTTcredentials";
const char* mqtt_password             = "yourMQTTcredentials";

const char* mqtt_deviceName           = "MaestroSwitch-";

//Maestro Switch
const char* mqtt_controlCmd           = "Home/Maestro/Switch/Cmd";         //MQTT JSON command data
const char* mqtt_controlStts          = "Home/Maestro/Switch/Stts";        //MQTT JSON status data

//Sensors
const char* mqtt_sensorJson           = "Home/Maestro/Switch/Json";        //MQTT JSON status data

const char* json_5vDim                = "Vcc5VDimmer";
const char* json_5vSw                 = "Vcc5VSwitch";

const char* json_ledPWM               = "pwmled"; 
const char* json_ledRem               = "remled";                           //Remote LEDs (switch and dimmer AVRs)
const char* json_ledEsp               = "espled";                           //WiFi LED

//LIGHT & FAN
const char* mqtt_lmpStts              = "Home/Maestro/Switch/LMP/Stts";     //MQTT JSON LIGHT status data
const char* mqtt_lmpCmd               = "Home/Maestro/Switch/LMP/Cmd";      //MQTT JSON LIGHT command data
const char* mqtt_fanStts              = "Home/Maestro/Switch/FAN/Stts";     //MQTT JSON FAN status data
const char* mqtt_fanCmd               = "Home/Maestro/Switch/FAN/Cmd";      //MQTT JSON FAN command data

const char* json_state                = "state";
const char* json_brightness           = "brightness";
const char* json_fade                 = "fade";

//Others
const char* json_parseFailed          = "parseObject() failed";
const char* json_resetReq             = "Reset Requested...";
const char* json_resetReboot          = "Rebooting...";
const char* json_reset                = "reset";
const char* json_resetAll             = "resetall";

const char* json_ssid                 = "wifi_ssid";
const char* json_rssi                 = "wifi_rssi";
const char* json_rssiPercent          = "wifi_percent";

const char* mqtt_cmdOn                = "ON";
const char* mqtt_cmdOff               = "OFF";

const char* mqtt_willTopic            = "Home/Maestro/Switch/LWT";
const char* mqtt_willOffline          = "offline";
const char* mqtt_willOnline           = "online";
const bool  mqtt_willRetain           = false;
const byte  mqtt_willQoS              = 0;

//Network settings
//MQTT Server IP Address
#define SIP1  192
#define SIP2  168
#define SIP3  0
#define SIP4  xxx

//Current device IP address
#define IP1  192
#define IP2  168
#define IP3  0
#define IP4  xxx

//Your gateway settings
#define GW1  192
#define GW2  168
#define GW3  0
#define GW4  1

//Your subnet settings
#define SN1  255
#define SN2  255
#define SN3  255
#define SN4  0
