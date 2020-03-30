// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- NETWORK SETUP ------------------------------------------
// ----------------------------------------------------------------------------------------------------
//You must put your own settings in here

const char* ssid                = "yourSSID";
const char* password            = "yourPassword";

const char* mqtt_server         = "192.168.0.xxx";
const char* mqtt_user           = "yourMQTTcredentials";
const char* mqtt_password       = "yourMQTTcredentials";

const char* mqtt_deviceName     = "WeatherStation-";

//Control
const char* mqtt_controlCmd     = "Outside/WStation/Cmd";
const char* mqtt_controlStts    = "Outside/WStation/Stts";

const char* json_state          = "state";
const char* json_reset          = "reset";
const char* json_rssi           = "wifi_rssi";
const char* json_rssiPercent    = "wifi_percent";

//Sensors
const char* mqtt_sensorJson     = "Outside/WStation/Json";         //MQTT JSON status data

const char* json_uv             = "uv";
const char* json_mq135          = "aq";

const char* json_temp           = "temp";
const char* json_hum            = "hum";
const char* json_vin            = "Vin";
const char* json_vcc            = "Vcc";

//Others
const char* json_parseFailed    = "parseObject() failed";
const char* json_resetReq       = "Reset Requested...";
const char* json_resetReboot    = "Rebooting...";

const char* mqtt_cmdOn          = "ON";
const char* mqtt_cmdOff         = "OFF";

const char* mqtt_willTopic      = "Outside/WStation/LWT";
const char* mqtt_willOffline    = "offline";
const char* mqtt_willOnline     = "online";
const bool  mqtt_willRetain     = false;
const byte  mqtt_willQoS        = 0;

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

//Your mac address settings
#define MC0  0xBA
#define MC1  0xDA
#define MC2  0x55
#define MC3  0x00
#define MC4  0x00
#define MC5  0x01
