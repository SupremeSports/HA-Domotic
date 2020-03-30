static byte gwip[] = { 192,168,0,2 };                                                  // Static Gateway IP Address
static byte subnet[] = { 255,255,255,0 };                                              // Static Subnet Mask
static byte myip[] = { 192,168,0,9 };                                                  // Static IP Address
static byte mymac[] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0x09 };                               // Static MAC Address - MUST NOT have 2 identical MAC address on same network

#define BUFLEN 32                                                                      // Define length of the reply string
#define ETHERBUFLEN 100                                                                // tcp/ip send and receive buffer

#define MAX_COMMANDS 3                                                                 // Maximum quantity of commands to be received

static int Ether_cspin = 10;                                                           // Chip select pin numper for ethernet shield
static int ListenPort = 5000;                                                          // Static port to listen

byte Ethernet::buffer[ETHERBUFLEN];                                                    // tcp/ip send and receive buffer

String commands[MAX_COMMANDS];                                                         // Stirns to send parsed data

char str[BUFLEN];                                                                      // Reply string for UDP request
