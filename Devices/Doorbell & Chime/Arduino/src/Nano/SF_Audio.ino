// ----------------------------------------------------------------------------------------------------
//------------------------------------------ SD/AUDIO SETUP -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initAudio()
{
  Sprintln("Init Audio...");

  tmrpcm.setVolume(startVolume);
  tmrpcm.speakerPin = speakerOut;
  digitalWrite(speakerOut, LOW);

  checkSD();
}

boolean checkSD()
{
  SD_CardOK = SD.begin(SD_ChipSelectPin);

  if (SD_CardOK)
    SD.exists(filename);
  
  return SD_CardOK;
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ SD/AUDIO PLAY -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void playAudio()
{
  if (selectedSound == 0)
  {
    if (ringRequested)
      playDefault(true);
      
    ringRequested = false;
    return;
  }

  bool isPlay = tmrpcm.isPlaying()==1;
  if (!isPlay && ringRequested)
    checkSD();
    
  if(!SD_CardOK)
  {
    if (ringRequested)
      playDefault(false);

    ringRequested = false;
    newI2Crequest = false;
    return;
  }
  
  if (isPlay)
  {
    if (activeVolume < selectedVolume)
      tmrpcm.setVolume(activeVolume++);
    
    ringRequested = false;
    newI2Crequest = false;
    return;
  }

  //Stop playback once music is done
  //Lower volume first
  if (activeVolume > startVolume)
    tmrpcm.setVolume(activeVolume--);
  if (activeVolume == startVolume)   
  {
    tmrpcm.stopPlayback();
    digitalWrite(speakerOut, LOW);
    digitalWrite(speakerEN, LOW);
  }

  if (!ringRequested)
    return;

  activeVolume = startVolume;
  tmrpcm.setVolume(startVolume);
  tmrpcm.quality(1);

  ringRequested = false;

  digitalWrite(speakerEN, HIGH);
  delay(100);
  tmrpcm.play(filename);
}

void playDefault(bool DingDong)
{
  int toneVolume = map(selectedVolume, 0, MAXVOL, 1000, 2);

  Sprintln("Playing default tune...");
  Sprint("Volume: ");
  Sprintln(toneVolume);

  int loops = DingDong ? MAX_COUNT_DD : MAX_COUNT;

  digitalWrite(speakerEN, HIGH);
  delay(100);
  
  for (int i=0; i<loops; i++)
  {
    tone_ = pgm_read_word_near(DingDong ? dingdong + i : melody + i);
    beat = pgm_read_word_near(DingDong ? ddbeats + i : beats + i);
    duration = beat * tempo; // Set up timing

    Sprint(i);
    Sprint(":");
    Sprint(beat);
    Sprint(" ");    
    Sprint(tone_);
    Sprint(" ");
    Sprintln(duration);

    playTone(selectedVolume>0 ? toneVolume : tone_);
    // A pause between notes...
    delayMicroseconds(pause);
  }

  digitalWrite(speakerEN, LOW);
}

void playTone(uint16_t vol)
{
  long elapsed_time = 0;
  int toneVol = tone_ / vol;

  if (tone_ > 0)
  {
    //Played for duration minus 'duration'
    //  Pulse speaker HIGH and LOW (simulated PWM)
    while (elapsed_time < duration)
    {
      // UP
      digitalWrite(speakerOut, toneVol>1 ? HIGH : LOW);
      delayMicroseconds(toneVol);

      // DOWN
      digitalWrite(speakerOut, LOW);
      delayMicroseconds(toneVol);

      //Add extra LOW time for the volume adjustment
      delayMicroseconds(tone_ - (2*(toneVol)));

      // Keep track of how long we pulsed
      elapsed_time += (tone_);
    }
  }
  else // Rest beat; loop times delay
  {
    for (int j = 0; j < rest_count; j++) // See NOTE on rest_count
      delayMicroseconds(duration);
  }
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SD/AUDIO COMMANDS -----------------------------------------
// ----------------------------------------------------------------------------------------------------
void soundRequest()
{
  if (dataIn[idByte] != soundID)
    return;

  Sprintln("Sound request received");

  selectedSound = dataIn[soundByte];
  selectedSound = constrain(selectedSound, 0, MAXSOUND);

  if (selectedSound == 0)
  {
    strcpy (filename, dingdong_name);
    strcat (filename, extension);
  }
  else
  {
    int one = (selectedSound%26 != 0) ? selectedSound % 26 : (selectedSound % 26) + 26;
    int two = ((selectedSound - one) / 26) + 1;
  
    strcpy (filename, alphabet[two]);
    strcat (filename, alphabet[one]);
    strcat (filename, extension);
  }

  Sprint("Selected sound: ");
  Sprintln(filename);
  
  selectedVolume = dataIn[volumeByte];
  selectedVolume = constrain(selectedVolume, 0, MAXVOL);
  Sprint("Volume: ");
  Sprintln(selectedVolume);

  ringRequested = dataIn[ringByte]==1;
}
