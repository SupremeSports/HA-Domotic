// ----------------------------------------------------------------------------------------------------	
// ------------------------------------------- NETWORK SETUP ------------------------------------------	
// ----------------------------------------------------------------------------------------------------	
//You must put your own settings in here	

const char* ssid                = "yourSSID";	
const char* password            = "yourPassword";	

const char* mqtt_server         = "192.168.0.xxx";	
const char* mqtt_user           = "yourMQTTcredentials";	
const char* mqtt_password       = "yourMQTTcredentials";	

const char* mqtt_deviceName     = "PoolClock-";

const char* mqtt_timeCmd        = "Home/Global/Time";             //Receive time in format YYYY:MM:DD:HH:MM:SS:
const char* mqtt_timeRequest    = "Home/Global/Time/Request";     //Request time update

//Control
const char* mqtt_controlStts    = "Outside/Pool/Clock/Stts";      //MQTT JSON CLOCK status data
const char* mqtt_controlCmd     = "Outside/Pool/Clock/Cmd";       //MQTT JSON CLOCK command data

const char* json_waterTemp      = "wtemp";
const char* json_waterPH        = "wph";
const char* json_outTemp        = "otemp";
const char* json_outHum         = "ohum";
const char* json_timeText       = "uptxt";
const char* json_tempText       = "dntxt";
const char* json_scroll         = "scroll";
const char* json_unavailable    = "unavailable";
const char* json_blank          = "";

//LEDs
const char* mqtt_ledStts        = "Outside/Pool/Clock/LED/Stts";  //MQTT JSON LIGHT status data
const char* mqtt_ledCmd         = "Outside/Pool/Clock/LED/Cmd";   //MQTT JSON LIGHT command data

const char* json_state          = "state";
const char* json_effect         = "effect";
const char* json_brightness     = "brightness";
const char* json_color          = "color";
const char* json_transition     = "transition";

//Sensors
const char* mqtt_sensorJson     = "Outside/Pool/Clock/Json";      //MQTT JSON status data

const char* json_reset          = "reset";
const char* json_rssi           = "rssi";
const char* json_rssiPercent    = "percent";

//Others
const char* json_parseID        = "Parse command ID failed";
const char* json_parseFailed    = "parseObject() failed";
const char* json_resetReq       = "Reset Requested...";
const char* json_resetReboot    = "Rebooting...";

const char* mqtt_cmdOn          = "ON";
const char* mqtt_cmdOff         = "OFF";

const char* mqtt_willTopic      = "Outside/Pool/Clock/LWT";
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
