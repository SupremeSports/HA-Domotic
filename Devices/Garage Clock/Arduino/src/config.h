// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- NETWORK SETUP ------------------------------------------
// ----------------------------------------------------------------------------------------------------
//You must put your own settings in here

const char* ssid                = "yourSSID";
const char* password            = "yourPassword";

const char* mqtt_server         = "192.168.0.xxx";
const char* mqtt_user           = "yourMQTTcredentials";
const char* mqtt_password       = "yourMQTTcredentials";

const char* mqtt_deviceName     = "GarageClock-";

//Time
const char* mqtt_timeCmd        = "Home/Global/Time";             //Receive time in format YYYY:MM:DD:HH:MM:SS:
const char* mqtt_timeRequest    = "Home/Global/Time/Request";     //Request time update

//Clock
const char* mqtt_clockStts      = "Home/NixieClock/Stts";         //MQTT Clock status data
const char* mqtt_clockCmd       = "Home/NixieClock/Cmd";          //MQTT Clock command data


#define WIFI_SSID "your SSID here"
#define WIFI_PASSWD "your password here"

//Network settings
//Modbus Server IP Address
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
#define GW4  xxx

//Your subnet settings
#define SN1  255
#define SN2  255
#define SN3  255
#define SN4  0
