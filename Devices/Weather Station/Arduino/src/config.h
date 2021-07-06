// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- NETWORK SETUP ------------------------------------------
// ----------------------------------------------------------------------------------------------------
//You must put your own settings in here

const char* ssid                      = "yourSSID";
const char* password                  = "yourPassword";

const char* mqtt_server               = "192.168.0.xxx";
const char* mqtt_user                 = "yourMQTTcredentials";
const char* mqtt_password             = "yourMQTTcredentials";

const char* mqtt_deviceName           = "WeatherStation-";

//Control
const char* mqtt_controlCmd           = "Weather/Control/Cmd";
const char* mqtt_controlStts          = "Weather/Control/Stts";

//Analog and Digital inputs
const char* mqtt_sensorsStts          = "Weather/Sensors/Stts";

const char* json_uv                   = "uv";
const char* json_mq135                = "aq";

//Sensors
const char* mqtt_sensorJson           = "Weather/Control/Json";         //MQTT JSON status data

const char* json_state                = "state";
const char* json_version              = "version";
const char* json_date                 = "date";

const char* json_tempin               = "temp_in";
const char* json_humin                = "hum_in";
const char* json_5v                   = "Vcc5V";
const char* json_12v                  = "Vcc12V";

const char* json_ssid                 = "wifi_ssid";
const char* json_rssi                 = "wifi_rssi";
const char* json_rssiPercent          = "wifi_percent";

//Others
const char* json_parseFailed          = "parseObject() failed";
const char* json_resetReq             = "Reset Requested...";
const char* json_resetReboot          = "Rebooting...";
const char* json_reset                = "reset";

const char* mqtt_cmdOn                = "ON";
const char* mqtt_cmdOff               = "OFF";

const char* mqtt_willTopic            = "Weather/Control/LWT";
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
//Serves also as DNS address
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
