// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- NETWORK SETUP ------------------------------------------
// ----------------------------------------------------------------------------------------------------
//You must put your own settings in here

const char* ssid                = "yourSSID";
const char* password            = "yourPassword";

const char* mqtt_server         = "192.168.0.xxx";
const char* mqtt_user           = "yourMQTTcredentials";
const char* mqtt_password       = "yourMQTTcredentials";

const char* mqtt_deviceName   = "PoolClock-";

//Time
const char* mqtt_timeValue    = "Home/Global/Time";             //Receive time in format YYYY:MM:DD:HH:MM:SS:
const char* mqtt_timeRequest  = "Home/Global/Time/Request";     //Request time update

const char* mqtt_waterTemp    = "Home/Pool/Clock/Water/Temp";
const char* mqtt_waterPH      = "Home/Pool/Clock/Water/PH";

const char* mqtt_outTemp      = "Home/Weather/Out/Temp";
const char* mqtt_outHum       = "Home/Weather/Out/Hum";

const char* mqtt_text_time    = "Home/Pool/Clock/Text/Time";
const char* mqtt_text_temp    = "Home/Pool/Clock/Text/Temp";

const char* mqtt_statusTopic  = "Home/Pool/Clock/Stts";         //MQTT JSON LIGHT status data
const char* mqtt_cmdTopic     = "Home/Pool/Clock/Cmd";          //MQTT JSON LIGHT command data
const char* mqtt_cmdOn        = "ON";
const char* mqtt_cmdOff       = "OFF";

const char* mqtt_willTopic    = "Home/Pool/Clock/LWT";
const char* mqtt_willOffline  = "offline";
const char* mqtt_willOnline   = "online";
const bool  mqtt_willRetain   = true;
const byte  mqtt_willQoS      = 0;

//Network settings
//MQTT Server IP Address
#define SIP1  192
#define SIP2  168
#define SIP3  0
#define SIP4  YYY

//Current device IP address
#define IP1  192
#define IP2  168
#define IP3  0
#define IP4  XXX

//Your gateway settings
#define GW1  192
#define GW2  168
#define GW3  0
#define GW4  XXX

//Your subnet settings
#define SN1  255
#define SN2  255
#define SN3  255
#define SN4  0
