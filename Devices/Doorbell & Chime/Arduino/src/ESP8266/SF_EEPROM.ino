// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- EEPROM INIT ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initEEPROM()
{
  EEPROM.begin(EEPROM_SIZE);
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- EEPROM FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readEEPROM()
{
  byte value_sound  = EEPROM.read(address_sound);
  byte value_volume  = EEPROM.read(address_volume);

  selectedSound = constrain(value_sound, 0, MAXSOUND);
  selectedVolume = constrain(value_volume, 0, MAXVOL);
  
  Sprintln("Read EEPROM: ");
  Sprint("Selected Sound: ");
  Sprintln(selectedSound);
  Sprint("Volume: ");
  Sprintln(selectedVolume);
}

void writeEEPROM()
{
  byte value_sound  = selectedSound;
  byte value_volume  = selectedVolume;
  
  Sprintln("Write EEPROM: ");
  Sprintln(value_sound);
  Sprintln(value_sound);
  
  EEPROM.put(address_sound, value_sound);
  EEPROM.put(address_volume, value_volume);

  EEPROM.commit();

  readEEPROM();
}

long EEPROMReadlong(long address)
{
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  Sprint(four);
  Sprint(" : ");
  Sprint(three);
  Sprint(" : ");
  Sprint(two);
  Sprint(" : ");
  Sprintln(one);
 
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void EEPROMWritelong(int address, long value)
{
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);
 
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);

  Sprint(four);
  Sprint(" : ");
  Sprint(three);
  Sprint(" : ");
  Sprint(two);
  Sprint(" : ");
  Sprintln(one);
}
