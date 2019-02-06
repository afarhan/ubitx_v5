/** Menus
 *  The Radio menus are accessed by tapping on the function button. 
 *  - The main loop() constantly looks for a button press and calls doMenu() when it detects
 *  a function button press. 
 *  - As the encoder is rotated, at every 10th pulse, the next or the previous menu
 *  item is displayed. Each menu item is controlled by it's own function.
 *  - Eache menu function may be called to display itself
 *  - Each of these menu routines is called with a button parameter. 
 *  - The btn flag denotes if the menu itme was clicked on or not.
 *  - If the menu item is clicked on, then it is selected,
 *  - If the menu item is NOT clicked on, then the menu's prompt is to be displayed
 */


/** A generic control to read variable values
*/
int getValueByKnob(int minimum, int maximum, int step_size,  int initial, char* prefix, char *postfix)
{
    int knob = 0;
    int knob_value;

    while (btnDown())
      active_delay(100);

    active_delay(200);
    knob_value = initial;
     
    strcpy(b, prefix);
    itoa(knob_value, c, 10);
    strcat(b, c);
    strcat(b, postfix);
    printLine2(b);
    active_delay(300);

    while(!btnDown() && digitalRead(PTT) == HIGH){

      knob = enc_read();
      if (knob != 0){
        if (knob_value > minimum && knob < 0)
          knob_value -= step_size;
        if (knob_value < maximum && knob > 0)
          knob_value += step_size;

        printLine2(prefix);
        itoa(knob_value, c, 10);
        strcpy(b, c);
        strcat(b, postfix);
        printLine1(b);
      }
      checkCAT();
    }

   return knob_value;
}

//# Menu: 1

int menuBand(int btn){
  int knob = 0;
  int band;
  unsigned long offset;

 // band = frequency/1000000l;
 // offset = frequency % 1000000l;
    
  if (!btn){
   printLine2("Band Select    \x7E");
   return;
  }

  printLine2("Band Select:");
  //wait for the button menu select button to be lifted)
  while (btnDown())
    active_delay(50);
  active_delay(50);    
  ritDisable();

  while(!btnDown()){

    knob = enc_read();
    if (knob != 0){
      /*
      if (band > 3 && knob < 0)
        band--;
      if (band < 30 && knob > 0)
        band++; 
      if (band > 10)
        isUSB = true;
      else
        isUSB = false;
      setFrequency(((unsigned long)band * 1000000l) + offset); */
      if (knob < 0 && frequency > 3000000l)
        setFrequency(frequency - 200000l);
      if (knob > 0 && frequency < 30000000l)
        setFrequency(frequency + 200000l);
      if (frequency > 10000000l)
        isUSB = true;
      else
        isUSB = false;
      updateDisplay();
    }
    checkCAT();
    active_delay(20);
  }

  while(btnDown())
    active_delay(50);
  active_delay(50);
  
  printLine2("");
  updateDisplay();
  menuOn = 0;
}

// Menu #2
void menuRitToggle(int btn){
  if (!btn){
    if (ritOn == 1)
      printLine2("RIT On \x7E Off");
    else
      printLine2("RIT Off \x7E On");
  }
  else {
    if (ritOn == 0){
      //enable RIT so the current frequency is used at transmit
      ritEnable(frequency);
      printLine2("RIT is On");
 
    }
    else{
      ritDisable();
      printLine2("RIT is Off");
    }
    menuOn = 0;
    active_delay(500);
    printLine2("");
    updateDisplay();
  }
}


//Menu #3
void menuVfoToggle(int btn){
  
  if (!btn){
    if (vfoActive == VFO_A)
      printLine2("VFO A \x7E B");
    else
      printLine2("VFO B \x7E A");
  }
  else {
    if (vfoActive == VFO_B){
      vfoB = frequency;
      isUsbVfoB = isUSB;
      EEPROM.put(VFO_B, frequency);
      if (isUsbVfoB)
        EEPROM.put(VFO_B_MODE, VFO_MODE_USB);
      else
        EEPROM.put(VFO_B_MODE, VFO_MODE_LSB);

      vfoActive = VFO_A;
//      printLine2("Selected VFO A  ");
      frequency = vfoA;
      isUSB = isUsbVfoA;
    }
    else {
      vfoA = frequency;
      isUsbVfoA = isUSB;
      EEPROM.put(VFO_A, frequency);
      if (isUsbVfoA)
        EEPROM.put(VFO_A_MODE, VFO_MODE_USB);
      else
        EEPROM.put(VFO_A_MODE, VFO_MODE_LSB);

      vfoActive = VFO_B;
//      printLine2("Selected VFO B  ");      
      frequency = vfoB;
      isUSB = isUsbVfoB;
    }
      
    ritDisable();
    setFrequency(frequency);
    updateDisplay();
    printLine2("");
      //exit the menu
    menuOn = 0;
  }
}

// Menu #4
void menuSidebandToggle(int btn){
  if (!btn){
    if (isUSB == true)
      printLine2("USB \x7E LSB");
    else
      printLine2("LSB \x7E USB");
  }
  else {
    if (isUSB == true){
      isUSB = false;
      printLine2("LSB Selected");
      active_delay(500);
      printLine2("");
    }
    else {
      isUSB = true;
      printLine2("USB Selected");
      active_delay(500);
      printLine2("");
    }
    //Added by KD8CEC
    if (vfoActive == VFO_B){
      isUsbVfoB = isUSB;
    }
    else {
      isUsbVfoB = isUSB;
    }
    updateDisplay();
    menuOn = 0;
  }
}

//Split communication using VFOA and VFOB by KD8CEC
//Menu #5
void menuSplitToggle(int btn){
  if (!btn){
    if (splitOn == 0)
      printLine2("Split Off \x7E On");
    else
      printLine2("Split On \x7E Off");
  }
  else {
    if (splitOn == 1){
      splitOn = 0;
      printLine2("Split ON");
    }
    else {
      splitOn = 1;
      if (ritOn == 1)
        ritOn = 0;
      printLine2("Split Off");
    }
    active_delay(500);
    printLine2("");
    updateDisplay();
    menuOn = 0;
  }
}

int menuCWSpeed(int btn){
    int knob = 0;
    int wpm;

    wpm = 1200/cwSpeed;
     
    if (!btn){
     strcpy(b, "CW: ");
     itoa(wpm,c, 10);
     strcat(b, c);
     strcat(b, " WPM     \x7E");
     printLine2(b);
     return;
    }

/*
    printLine1("Press FN to Set");
    strcpy(b, "5:CW>");
    itoa(wpm,c, 10);
    strcat(b, c);
    strcat(b, " WPM");
    printLine2(b);
    active_delay(300);

    while(!btnDown() && digitalRead(PTT) == HIGH){

      knob = enc_read();
      if (knob != 0){
        if (wpm > 3 && knob < 0)
          wpm--;
        if (wpm < 50 && knob > 0)
          wpm++;

        strcpy(b, "5:CW>");
        itoa(wpm,c, 10);
        strcat(b, c);
        strcat(b, " WPM");
        printLine2(b);
      }
      //abort if this button is down
      if (btnDown())
        //re-enable the clock1 and clock 2
        break;
      checkCAT();
    }
  */
    wpm = getValueByKnob(1, 100, 1,  wpm, "CW: ", " WPM>");
  
    printLine2("CW Speed set!");
    cwSpeed = 1200/wpm;
    EEPROM.put(CW_SPEED, cwSpeed);
    active_delay(500);
    
    printLine2("");
    updateDisplay();
    menuOn = 0;
}

void menuExit(int btn){

  if (!btn){
      printLine2("Exit Menu      \x7E");
  }
  else{
      printLine2("Exiting...");
      active_delay(500);
      printLine2("");
      updateDisplay();
      menuOn = 0;
  }
}

/**
 * The calibration routines are not normally shown in the menu as they are rarely used
 * They can be enabled by choosing this menu option
 */
int menuSetup(int btn){
  if (!btn){
    if (!modeCalibrate)
      printLine2("Settings       \x7E");
    else
      printLine2("Settings \x7E Off");
  }else {
    if (!modeCalibrate){
      modeCalibrate = true;
      printLine2("Settings On");
    }
    else {
      modeCalibrate = false;
      printLine2("Settings Off");      
    }

   while(btnDown())
    active_delay(100);
   active_delay(500);
   printLine2("");
   return 10;
  }
  return 0;
}

 //this is used by the si5351 routines in the ubitx_5351 file
extern int32_t calibration;
extern uint32_t si5351bx_vcoa;

int calibrateClock(){
  int knob = 0;
  int32_t prev_calibration;


  //keep clear of any previous button press
  while (btnDown())
    active_delay(100);
  active_delay(100);
   
  digitalWrite(TX_LPF_A, 0);
  digitalWrite(TX_LPF_B, 0);
  digitalWrite(TX_LPF_C, 0);

  prev_calibration = calibration;
  calibration = 0;

  isUSB = true;

  //turn off the second local oscillator and the bfo
  si5351_set_calibration(calibration);
  startTx(TX_CW);
  si5351bx_setfreq(2, 10000000l); 
  
  strcpy(b, "#1 10 MHz cal:");
  ltoa(calibration/8750, c, 10);
  strcat(b, c);
  printLine2(b);     

  while (!btnDown())
  {

    if (digitalRead(PTT) == LOW && !keyDown)
      cwKeydown();
    if (digitalRead(PTT)  == HIGH && keyDown)
      cwKeyUp();
      
    knob = enc_read();

    if (knob > 0)
      calibration += 875;
    else if (knob < 0)
      calibration -= 875;
    else 
      continue; //don't update the frequency or the display
      
    si5351_set_calibration(calibration);
    si5351bx_setfreq(2, 10000000l);
    strcpy(b, "#1 10 MHz cal:");
    ltoa(calibration/8750, c, 10);
    strcat(b, c);
    printLine2(b);     
  }

  cwTimeout = 0;
  keyDown = 0;
  stopTx();

  printLine2("Calibration set!");
  EEPROM.put(MASTER_CAL, calibration);
  initOscillators();
  setFrequency(frequency);    
  updateDisplay();

  while(btnDown())
    active_delay(50);
  active_delay(100);
}

int menuSetupCalibration(int btn){
  int knob = 0;
  int32_t prev_calibration;
   
  if (!btn){
    printLine2("Setup:Calibrate\x7E");
    return 0;
  }

  printLine1("Press PTT & tune");
  printLine2("to exactly 10 MHz");
  active_delay(2000);
  calibrateClock();
}

void printCarrierFreq(unsigned long freq){

  memset(c, 0, sizeof(c));
  memset(b, 0, sizeof(b));

  ultoa(freq, b, DEC);
  
  strncat(c, b, 2);
  strcat(c, ".");
  strncat(c, &b[2], 3);
  strcat(c, ".");
  strncat(c, &b[5], 1);
  printLine2(c);    
}

void menuSetupCarrier(int btn){
  int knob = 0;
  unsigned long prevCarrier;
   
  if (!btn){
      printLine2("Setup:BFO      \x7E");
    return;
  }

  prevCarrier = usbCarrier;
  printLine1("Tune to best Signal");  
  printLine2("Press to confirm. ");
  active_delay(1000);

  usbCarrier = 11053000l;
  si5351bx_setfreq(0, usbCarrier);
  printCarrierFreq(usbCarrier);

  //disable all clock 1 and clock 2 
  while (!btnDown()){
    knob = enc_read();

    if (knob > 0)
      usbCarrier -= 50;
    else if (knob < 0)
      usbCarrier += 50;
    else
      continue; //don't update the frequency or the display
      
    si5351bx_setfreq(0, usbCarrier);
    printCarrierFreq(usbCarrier);
    
    active_delay(100);
  }

  printLine2("Carrier set!    ");
  EEPROM.put(USB_CAL, usbCarrier);
  active_delay(1000);
  
  si5351bx_setfreq(0, usbCarrier);          
  setFrequency(frequency);    
  updateDisplay();
  printLine2("");
  menuOn = 0; 
}

void menuSetupCwTone(int btn){
  int knob = 0;
  int prev_sideTone;
     
  if (!btn){
    printLine2("Setup:CW Tone  \x7E");
    return;
  }

  prev_sideTone = sideTone;
  printLine1("Tune CW tone");  
  printLine2("PTT to confirm. ");
  active_delay(1000);
  tone(CW_TONE, sideTone);

  //disable all clock 1 and clock 2 
  while (digitalRead(PTT) == HIGH && !btnDown())
  {
    knob = enc_read();

    if (knob > 0 && sideTone < 2000)
      sideTone += 10;
    else if (knob < 0 && sideTone > 100 )
      sideTone -= 10;
    else
      continue; //don't update the frequency or the display
        
    tone(CW_TONE, sideTone);
    itoa(sideTone, b, 10);
    printLine2(b);

    checkCAT();
    active_delay(20);
  }
  noTone(CW_TONE);
  //save the setting
  if (digitalRead(PTT) == LOW){
    printLine2("Sidetone set!    ");
    EEPROM.put(CW_SIDETONE, sideTone);
    active_delay(2000);
  }
  else
    sideTone = prev_sideTone;
    
  printLine2("");  
  updateDisplay(); 
  menuOn = 0; 
}

void menuSetupCwDelay(int btn){
  int knob = 0;
  int prev_cw_delay;

  if (!btn){
    printLine2("Setup:CW Delay \x7E");
    return;
  }

  active_delay(500);
  prev_cw_delay = cwDelayTime;
  cwDelayTime = getValueByKnob(10, 1000, 50,  cwDelayTime, "CW Delay>", " msec");

  printLine1("CW Delay Set!");  
  printLine2("");
  active_delay(500);
  updateDisplay();
  menuOn = 0;
}

void menuSetupKeyer(int btn){
  int tmp_key, knob;
  
  if (!btn){
    if (!Iambic_Key)
      printLine2("Setup:CW(Hand)\x7E");
    else if (keyerControl & IAMBICB)
      printLine2("Setup:CW(IambA)\x7E");
    else 
      printLine2("Setup:CW(IambB)\x7E");    
    return;
  }
  
  active_delay(500);

  if (!Iambic_Key)
    tmp_key = 0; //hand key
  else if (keyerControl & IAMBICB)
    tmp_key = 2; //Iambic B
  else 
    tmp_key = 1;
 
  while (!btnDown())
  {
    knob = enc_read();
    if (knob < 0 && tmp_key > 0)
      tmp_key--;
    if (knob > 0)
      tmp_key++;

    if (tmp_key > 2)
      tmp_key = 0;
      
    if (tmp_key == 0)
      printLine1("Hand Key?");
    else if (tmp_key == 1)
      printLine1("Iambic A?");
    else if (tmp_key == 2)  
      printLine1("Iambic B?");  
  }

  active_delay(500);
  if (tmp_key == 0)
    Iambic_Key = false;
  else if (tmp_key == 1){
    Iambic_Key = true;
    keyerControl &= ~IAMBICB;
  }
  else if (tmp_key == 2){
    Iambic_Key = true;
    keyerControl |= IAMBICB;
  }
  
  EEPROM.put(CW_KEY_TYPE, tmp_key);
  
  printLine1("Keyer Set!");
  active_delay(600);
  printLine1("");

  //Added KD8CEC
  printLine2("");
  updateDisplay(); 
  menuOn = 0;  
}

void menuReadADC(int btn){
  int adc;
  
  if (!btn){
    printLine2("6:Setup>Read ADC>");
    return;
  }
  delay(500);

  while (!btnDown()){
    adc = analogRead(ANALOG_KEYER);
    itoa(adc, b, 10);
    printLine1(b);
  }

  printLine1("");
  updateDisplay();
}

void doMenu(){
  int select=0, i,btnState;

  //wait for the button to be raised up
  while(btnDown())
    active_delay(50);
  active_delay(50);  //debounce
  
  menuOn = 2;
  
  while (menuOn){
    i = enc_read();
    btnState = btnDown();

    if (i > 0){
      if (modeCalibrate && select + i < 150)
        select += i;
      if (!modeCalibrate && select + i < 80)
        select += i;
    }
    if (i < 0 && select - i >= 0)
      select += i;      //caught ya, i is already -ve here, so you add it

    if (select < 10)
      menuBand(btnState);
    else if (select < 20)
      menuRitToggle(btnState);
    else if (select < 30)
      menuVfoToggle(btnState);
    else if (select < 40)
      menuSidebandToggle(btnState);
    else if (select < 50)
      menuSplitToggle(btnState);
    else if (select < 60)
      menuCWSpeed(btnState);
    else if (select < 70)
      select += menuSetup(btnState);
    else if (select < 80 && !modeCalibrate)
      menuExit(btnState);
    else if (select < 90 && modeCalibrate)
      menuSetupCalibration(btnState);   //crystal
    else if (select < 100 && modeCalibrate)
      menuSetupCarrier(btnState);       //lsb
    else if (select < 110 && modeCalibrate)
      menuSetupCwTone(btnState);
    else if (select < 120 && modeCalibrate)
      menuSetupCwDelay(btnState);
    else if (select < 130 && modeCalibrate)
      menuReadADC(btnState);
    else if (select < 140 && modeCalibrate)
        menuSetupKeyer(btnState);
    else
      menuExit(btnState);  
  }

  //debounce the button
  while(btnDown())
    active_delay(50);
  active_delay(50);

  checkCAT();
}

