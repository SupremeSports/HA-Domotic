// ----------------------------------------------------------------------------------------------------	
// ------------------------------------------- NETWORK SETUP ------------------------------------------	
// ----------------------------------------------------------------------------------------------------	
//You must put your own settings in here	

const int   ssid_qty                  = 4;
const char* ssid[ssid_qty]            = {"yourSSID1", "yourSSID2", "yourSSID3", "yourSSID4"};
const char* password[ssid_qty]        = {"yourPassword1", "yourPassword2", "yourPassword3", "yourPassword4"};

const char* mqtt_server               = "192.168.0.xxx";	
const char* mqtt_user                 = "yourMQTTcredentials";	
const char* mqtt_password             = "yourMQTTcredentials";	

const char* mqtt_deviceName           = "PoolClock-";

const char* mqtt_timeCmd              = "Home/Global/Time";             //Receive time in format YYYY:MM:DD:HH:MM:SS:
const char* mqtt_timeRequest          = "Home/Global/Time/Request";     //Request time update

//Control
const char* mqtt_controlStts          = "Pool/Clock/Stts";              //MQTT JSON CLOCK status data
const char* mqtt_controlCmd           = "Pool/Clock/Cmd";               //MQTT JSON CLOCK command data

const char* json_waterTemp            = "wtemp";
const char* json_waterPH              = "wph";
const char* json_outTemp              = "otemp";
const char* json_outHum               = "ohum";
const char* json_timeText             = "uptxt";
const char* json_tempText             = "dntxt";
const char* json_scroll               = "scroll";

//LEDs
const char* mqtt_ledStts              = "Pool/Clock/LED/Stts";          //MQTT JSON LIGHT status data
const char* mqtt_ledCmd               = "Pool/Clock/LED/Cmd";           //MQTT JSON LIGHT command data

const char* json_effect               = "effect";
const char* json_brightness           = "brightness";
const char* json_color                = "color";
const char* json_transition           = "transition";

//Sensors
const char* mqtt_sensorJson           = "Pool/Clock/Json";              //MQTT JSON status data

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

const char* mqtt_willTopic            = "Pool/Clock/LWT";
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
