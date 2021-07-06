// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- NETWORK SETUP ------------------------------------------
// ----------------------------------------------------------------------------------------------------
//You must put your own settings in here

const char* ssid                      = "yourSSID";
const char* password                  = "yourPassword";
const char* passwordWrong             = "TEST1234";

const char* mqtt_server               = "192.168.1.10";
const char* mqtt_user                 = "yourMQTTcredentials";
const char* mqtt_password             = "yourMQTTcredentials";

#if defined(Switch1)
const char* mqtt_deviceName           = "MaestroS00001-";
#elif defined(Switch2)
const char* mqtt_deviceName           = "MaestroS00002-";
#elif defined(Switch3)
const char* mqtt_deviceName           = "MaestroS00003-";
#endif

//Maestro Switch
#if defined(Switch1)
const char* mqtt_controlStts          = "Home/Maestro/S00001/Stts";        //MQTT JSON status data
const char* mqtt_controlCmd           = "Home/Maestro/S00001/Cmd";         //MQTT JSON command data
#elif defined(Switch2)
const char* mqtt_controlStts          = "Home/Maestro/S00002/Stts";        //MQTT JSON status data
const char* mqtt_controlCmd           = "Home/Maestro/S00002/Cmd";         //MQTT JSON command data
#elif defined(Switch3)
const char* mqtt_controlStts          = "Home/Maestro/S00003/Stts";        //MQTT JSON status data
const char* mqtt_controlCmd           = "Home/Maestro/S00003/Cmd";         //MQTT JSON command data
#endif

//LIGHT & FAN
#if defined(Switch1)
const char* mqtt_lmpStts              = "Home/Maestro/S00001/LMP/Stts";     //MQTT JSON LIGHT status data
const char* mqtt_lmpCmd               = "Home/Maestro/S00001/LMP/Cmd";      //MQTT JSON LIGHT command data
const char* mqtt_fanStts              = "Home/Maestro/S00001/FAN/Stts";     //MQTT JSON FAN status data
const char* mqtt_fanCmd               = "Home/Maestro/S00001/FAN/Cmd";      //MQTT JSON FAN command data
#elif defined(Switch2)
const char* mqtt_lmpStts              = "Home/Maestro/S00002/LMP/Stts";     //MQTT JSON LIGHT status data
const char* mqtt_lmpCmd               = "Home/Maestro/S00002/LMP/Cmd";      //MQTT JSON LIGHT command data
const char* mqtt_fanStts              = "Home/Maestro/S00002/FAN/Stts";     //MQTT JSON FAN status data
const char* mqtt_fanCmd               = "Home/Maestro/S00002/FAN/Cmd";      //MQTT JSON FAN command data
#elif defined(Switch3)
const char* mqtt_lmpStts              = "Home/Maestro/S00003/LMP/Stts";     //MQTT JSON LIGHT status data
const char* mqtt_lmpCmd               = "Home/Maestro/S00003/LMP/Cmd";      //MQTT JSON LIGHT command data
const char* mqtt_fanStts              = "Home/Maestro/S00003/FAN/Stts";     //MQTT JSON FAN status data
const char* mqtt_fanCmd               = "Home/Maestro/S00003/FAN/Cmd";      //MQTT JSON FAN command data
#endif

const char* json_brightness           = "brightness";
const char* json_fade                 = "fade";

//Sensors
#if defined(Switch1)
const char* mqtt_sensorJson           = "Home/Maestro/S00001/Json";
#elif defined(Switch2)
const char* mqtt_sensorJson           = "Home/Maestro/S00002/Json";
#elif defined(Switch3)
const char* mqtt_sensorJson           = "Home/Maestro/S00003/Json";
#endif

const char* json_state                = "state";
const char* json_version              = "version";
const char* json_date                 = "date";

const char* json_tempin               = "temp_in";
const char* json_humin                = "hum_in";
const char* json_tempout              = "temp_out";
const char* json_humout               = "hum_out";

const char* json_5vDim                = "Vcc5VDimmer";
const char* json_5vSw                 = "Vcc5VSwitch";

const char* json_ledPWM               = "pwmled"; 
const char* json_ledRem               = "remled";                           //Remote LEDs (switch and dimmer AVRs)
const char* json_ledEsp               = "espled";                           //WiFi LED

const char* json_ssid                 = "wifi_ssid";
const char* json_rssi                 = "wifi_rssi";
const char* json_rssiPercent          = "wifi_percent";

const char* json_hb_in                = "heartbeat_in";
const char* json_hb_out               = "heartbeat_out";

//Others
const char* json_parseFailed          = "parseObject() failed";
const char* json_resetReq             = "Reset Requested...";
const char* json_resetReboot          = "Rebooting...";
const char* json_reset                = "reset";
const char* json_resetAll             = "resetall";

const char* mqtt_cmdOn                = "ON";
const char* mqtt_cmdOff               = "OFF";

#if defined(Switch1)
const char* mqtt_willTopic            = "Home/Maestro/S00001/LWT";
#elif defined(Switch2)
const char* mqtt_willTopic            = "Home/Maestro/S00002/LWT";
#elif defined(Switch3)
const char* mqtt_willTopic            = "Home/Maestro/S00003/LWT";
#endif

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
