// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- NETWORK SETUP ------------------------------------------
// ----------------------------------------------------------------------------------------------------
//You must put your own settings in here

const char* ssid                = "yourSSID";
const char* password            = "yourPassword";

const char* mqtt_server         = "192.168.0.xxx";
const char* mqtt_user           = "yourMQTTcredentials";
const char* mqtt_password       = "yourMQTTcredentials";

const char* mqtt_deviceName     = "DoorChimeTF-";

//Control
const char* mqtt_controlStts    = "Home/Chime/Top/Stts";            //MQTT JSON control status data
const char* mqtt_controlCmd     = "Home/Chime/Top/Cmd";             //MQTT JSON control command data

const char* json_lock           = "lock";
const char* json_unlock         = "unlock";
const char* json_mode           = "mode";
const char* json_right          = "right";

//Sensors
const char* mqtt_sensorJson     = "Home/Chime/Top/Json";            //MQTT JSON sensor data

const char* json_state          = "state";
const char* json_sound          = "sound";
const char* json_volume         = "volume";
const char* json_reset          = "reset";
const char* json_resetnano      = "reset_nano";
const char* json_rssi           = "wifi_rssi";
const char* json_rssiPercent    = "wifi_percent";

//Others
const char* json_parseFailed    = "parseObject() failed";
const char* json_resetReq       = "Reset Requested...";
const char* json_resetReboot    = "Rebooting...";

const char* mqtt_cmdOn          = "ON";
const char* mqtt_cmdOff         = "OFF";

const char* mqtt_willTopic      = "Home/Chime/Top/LWT";
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
