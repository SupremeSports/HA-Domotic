// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- NETWORK SETUP ------------------------------------------
// ----------------------------------------------------------------------------------------------------
//You must put your own settings in here

const char* ssid                = "yourSSID";
const char* password            = "yourPassword";

const char* mqtt_server         = "192.168.0.xxx";
const char* mqtt_user           = "yourMQTTcredentials";
const char* mqtt_password       = "yourMQTTcredentials";

const char* mqtt_deviceName     = "NixieClock-";

//Time
const char* mqtt_timeCmd        = "Home/Global/Time";             //Receive time in format YYYY:MM:DD:HH:MM:SS:
const char* mqtt_timeRequest    = "Home/Global/Time/Request";     //Request time update

//Clock
const char* mqtt_controlStts    = "Home/NixieClock/Stts";         //MQTT Clock status data
const char* mqtt_controlCmd     = "Home/NixieClock/Cmd";          //MQTT Clock command data

//Booleans
const char* json_hourMode       = "hrm";
const char* json_blankLead      = "bkl";
const char* json_scrollback     = "scb";
const char* json_suppressACP    = "acp";
const char* json_useFade        = "fde";
const char* json_useLDR         = "ldr";
const char* json_slotsMode      = "slt";
//Integers
const char* json_dateFormat     = "dte";
const char* json_dayBlanking    = "dbk";
const char* json_blankFrom      = "bbk";
const char* json_blankTo        = "ebk";
const char* json_fadeSteps      = "fds";
const char* json_scrollSteps    = "scs";
const char* json_blankMode      = "bkm";
const char* json_minDim         = "dim";

//Sensors
const char* mqtt_clockJson      = "Home/NixieClock/Json";         //MQTT JSON status data

const char* json_casetemp       = "case_temp";
const char* json_reset          = "reset";
const char* json_rssi           = "wifi_rssi";
const char* json_rssiPercent    = "wifi_percent";

//LEDs
const char* mqtt_ledStts        = "Home/NixieClock/LED/Stts";     //MQTT JSON LIGHT status data
const char* mqtt_ledCmd         = "Home/NixieClock/LED/Cmd";      //MQTT JSON LIGHT command data

const char* json_state          = "state";
const char* json_effect         = "effect";
const char* json_speed          = "speed";
const char* json_color          = "color";

//Others
const char* json_parseFailed    = "parseObject() failed";
const char* json_resetReq       = "Reset Requested...";
const char* json_resetReboot    = "Rebooting...";

const char* mqtt_cmdOn          = "ON";
const char* mqtt_cmdOff         = "OFF";

const char* mqtt_willTopic      = "Home/NixieClock/LWT";
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
