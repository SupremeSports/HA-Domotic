// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- NETWORK SETUP ------------------------------------------
// ----------------------------------------------------------------------------------------------------
//You must put your own settings in here

const int   ssid_qty                  = 4;
const char* ssid[ssid_qty]            = {"yourSSID1", "yourSSID2", "yourSSID3", "yourSSID"};
const char* password[ssid_qty]        = {"yourPassword1", "yourPassword2", "yourPassword3", "yourPassword4"};

const char* mqtt_server               = "192.168.0.xxx";
const char* mqtt_user                 = "yourMQTTcredentials";
const char* mqtt_password             = "yourMQTTcredentials";

const char* mqtt_deviceName           = "LivingTherm";

//Time
const char* mqtt_timeCmd              = "Home/Global/Time";             //Receive time in format YYYY:MM:DD:HH:MM:SS:
const char* mqtt_timeRequest          = "Home/Global/Time/Request";     //Request time update

//Control
const char* mqtt_controlCmd           = "Home/Living/Therm/Cmd";
const char* mqtt_controlStts          = "Home/Living/Therm/Stts";

const char* json_passcode             = "passcode";

//HVAC
const char* mqtt_powerCmd             = "Home/Living/Hvac/Power/Cmd";
const char* mqtt_modeCmd              = "Home/Living/Hvac/Mode/Cmd";
const char* mqtt_tempCmd              = "Home/Living/Hvac/Temp/Cmd";
const char* mqtt_fanCmd               = "Home/Living/Hvac/Fan/Cmd";
const char* mqtt_swingCmd             = "Home/Living/Hvac/Swing/Cmd";

const char* mqtt_hvacStts             = "Home/Living/Hvac/Stts";          //Status data from HVAC system
const char* mqtt_hvacJson             = "Home/Living/Hvac/Json";          //JSON data from HVAC system

const char* json_state                = "state";
const char* json_mode                 = "mode";
const char* json_temp                 = "temp";
const char* json_fan                  = "fan";
const char* json_swing                = "swing";

//Sensors
const char* mqtt_sensorJson           = "Home/Living/Therm/Json";         //MQTT JSON status data

const char* json_airTemp              = "air_temp";
const char* json_roomTemp             = "room_temp";
const char* json_roomHum              = "room_hum";

const char* json_5v                   = "Vcc5V";

//Others
const char* json_parseFailed          = "parseObject() failed";
const char* json_resetReq             = "Reset Requested...";
const char* json_resetReboot          = "Rebooting...";
const char* json_reset                = "reset";

const char* json_ssid                 = "wifi_ssid";
const char* json_rssi                 = "wifi_rssi";
const char* json_rssiPercent          = "wifi_percent";

const char* mqtt_cmdOn                = "ON";
const char* mqtt_cmdOff               = "OFF";

const char* mqtt_willTopic            = "Home/Living/Therm/LWT";
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
