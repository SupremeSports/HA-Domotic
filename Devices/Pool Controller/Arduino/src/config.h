// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- NETWORK SETUP ------------------------------------------
// ----------------------------------------------------------------------------------------------------
//You must put your own settings in here

const char* ssid                      = "yourSSID";
const char* password                  = "yourPassword";

const char* mqtt_server               = "192.168.0.xxx";
const char* mqtt_user                 = "yourMQTTcredentials";
const char* mqtt_password             = "yourMQTTcredentials";

const char* mqtt_deviceName           = "PoolControl-";

//Pool Control
const char* mqtt_controlCmd           = "Pool/Control/Cmd";             //MQTT JSON command data
const char* mqtt_controlStts          = "Pool/Control/Stts";            //MQTT JSON status data

const char* json_drain                = "drain";
const char* json_slide                = "slide";
const char* json_heatp                = "heatp";

//Relay Board
const char* mqtt_relayCmd             = "Pool/Control/Relay/Cmd";
const char* mqtt_relayStts            = "Pool/Control/Relay/Stts";

const char* jsonRelay[16]             = {
                                        "r1",
                                        "r2",
                                        "r3",
                                        "r4",
                                        "r5",
                                        "r6",
                                        "r7",
                                        "r8",
                                        "r9",
                                        "r10",
                                        "r11",
                                        "r12",
                                        "r13",
                                        "r14",
                                        "r15",
                                        "r16",
                                        };

//Analog and Digital inputs
const char* mqtt_sensorsStts          = "Pool/Control/Sensors/Stts";

const char* json_hilvl                = "hilvl";
const char* json_lolvl                = "lolvl";

const char* json_wtemp                = "wtemp";
const char* json_htemp                = "htemp";
const char* json_wpsi                 = "wpsi";
const char* json_wph                  = "wph";

const char* json_ptemp                = "ptemp";

//Control Door Lock
const char* mqtt_doorCmd              = "Pool/Door/Cabin/Cmd";
const char* mqtt_doorStts             = "Pool/Door/Cabin/Stts";

const char* json_lock                 = "lock";
const char* json_unlock               = "unlock";

const char* json_opened               = "opened";
const char* json_closed               = "closed";
const char* json_locked               = "locked";
const char* json_unlocked             = "unlocked";
const char* json_unknown              = "unknown";
                        
//Sensors
const char* mqtt_sensorJson           = "Pool/Control/Json";            //MQTT JSON status data

const char* json_state                = "state";

const char* json_tempin               = "temp_in";
const char* json_humin                = "hum_in";
const char* json_tempout              = "temp_out";
const char* json_humout               = "hum_out";

const char* json_5v                   = "Vcc5V";
const char* json_12v                  = "Vcc12V";

//Others
const char* json_parseFailed          = "parseObject() failed";
const char* json_resetReq             = "Reset Requested...";
const char* json_resetReboot          = "Rebooting...";
const char* json_reset                = "reset";

const char* json_rssi                 = "wifi_rssi";
const char* json_rssiPercent          = "wifi_percent";

const char* mqtt_cmdOn                = "ON";
const char* mqtt_cmdOff               = "OFF";

const char* mqtt_willTopic            = "Pool/Control/LWT";
const char* mqtt_willOffline          = "offline";
const char* mqtt_willOnline           = "online";
const bool  mqtt_willRetain           = false;
const byte  mqtt_willQoS              = 0;

//Network settings
//MQTT Server IP Address
#define SIP1  192
#define SIP2  168
#define SIP3  0
#define SIP4  YYY

//Current device IP address
//Serves also as DNS address
#define IP1  192
#define IP2  168
#define IP3  0
#define IP4  XXX

//Your gateway settings
#define GW1  192
#define GW2  168
#define GW3  0
#define GW4  ZZZ

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
#define MC4  0xFE
#define MC5  0x80
