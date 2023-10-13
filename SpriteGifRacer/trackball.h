bool trackball_interrupted = false;
int16_t trackball_up_count = 0;
int16_t trackball_down_count = 0;
int16_t trackball_left_count = 0;
int16_t trackball_right_count = 0;
int16_t trackball_click_count = 0;
void IRAM_ATTR ISR_up()
{
  // Serial.println("ISR_up");
  trackball_interrupted = true;
  ++trackball_up_count;
}
void IRAM_ATTR ISR_down()
{
  // Serial.println("ISR_down");
  trackball_interrupted = true;
  ++trackball_down_count;
}
void IRAM_ATTR ISR_left()
{
  // Serial.println("ISR_left");
  trackball_interrupted = true;
  ++trackball_left_count;
}
void IRAM_ATTR ISR_right()
{
  // Serial.println("ISR_right");
  trackball_interrupted = true;
  ++trackball_right_count;
}
void IRAM_ATTR ISR_click()
{
  // Serial.println("ISR_click");
  trackball_interrupted = true;
  ++trackball_click_count;
}

void init_trackball(uint8_t rotation)
{
  if (rotation == 1)
  {
    // Init trackball
    pinMode(PIN_GS3, INPUT_PULLUP);
    attachInterrupt(PIN_GS3, ISR_up, FALLING);

    pinMode(PIN_GS2, INPUT_PULLUP);
    attachInterrupt(PIN_GS2, ISR_right, FALLING);

    pinMode(PIN_GS1, INPUT_PULLUP);
    attachInterrupt(PIN_GS1, ISR_down, FALLING);

    pinMode(PIN_GS4, INPUT_PULLUP);
    attachInterrupt(PIN_GS4, ISR_left, FALLING);
  }
  else
  {
    // Init trackball
    pinMode(PIN_GS4, INPUT_PULLUP);
    attachInterrupt(PIN_GS4, ISR_up, FALLING);

    pinMode(PIN_GS3, INPUT_PULLUP);
    attachInterrupt(PIN_GS3, ISR_right, FALLING);

    pinMode(PIN_GS2, INPUT_PULLUP);
    attachInterrupt(PIN_GS2, ISR_down, FALLING);

    pinMode(PIN_GS1, INPUT_PULLUP);
    attachInterrupt(PIN_GS1, ISR_left, FALLING);
  }

  pinMode(PIN_BOOT_BTN, INPUT_PULLUP);
  attachInterrupt(PIN_BOOT_BTN, ISR_click, FALLING);
}
