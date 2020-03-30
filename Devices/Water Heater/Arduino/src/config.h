// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- NETWORK SETUP ------------------------------------------
// ----------------------------------------------------------------------------------------------------
//You must put your own settings in here

const char* ssid                = "yourSSID";
const char* password            = "yourPassword";

const char* mqtt_server         = "192.168.0.xxx";
const char* mqtt_user           = "yourMQTTcredentials";
const char* mqtt_password       = "yourMQTTcredentials";

const char* mqtt_deviceName     = "WaterHeater-";

//Control
const char* mqtt_controlCmd     = "Home/WaterHeater/Cmd";
const char* mqtt_controlStts    = "Home/WaterHeater/Stts";

//Sensors
const char* mqtt_sensorJson     = "Home/WaterHeater/Json";         //MQTT JSON status data

const char* json_state          = "state";
const char* json_tempin         = "temp_in";
const char* json_tempout        = "temp_out";
const char* json_flow           = "flow";
const char* json_reset          = "reset";
const char* json_rssi           = "wifi_rssi";
const char* json_rssiPercent    = "wifi_percent";

//Others
const char* json_parseFailed    = "parseObject() failed";
const char* json_resetReq       = "Reset Requested...";
const char* json_resetReboot    = "Rebooting...";

const char* mqtt_cmdOn          = "ON";
const char* mqtt_cmdOff         = "OFF";

const char* mqtt_willTopic      = "Home/WaterHeater/LWT";
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
