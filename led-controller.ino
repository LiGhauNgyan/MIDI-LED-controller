#include <FastLED.h>
#define LED_PIN 7
// set number of leds of your light bar
#define NUM_LEDS 144
#define NUM_KEYS (NUM_LEDS >> 1)
CRGB leds[NUM_LEDS] = {0};
uint8_t recvlen, tmp_val;
uint8_t recvbuf[SERIAL_RX_BUFFER_SIZE];
const uint32_t time_step = 8192;
// adjust prelude time and remain time here, unit: millisecond
const uint32_t prelude_t = 1200, remain_t = 200;
const uint16_t prelude_time = 1000 * prelude_t / time_step, remain_time = 1000 * remain_t / time_step;
uint8_t remain_countdown[NUM_KEYS];
uint8_t prelude_countdown[NUM_KEYS];
uint32_t next_timer;
uint16_t tick, event_queue[64];
uint8_t queue_head, queue_tail, event_key[64];
const uint8_t key_type[12] = {1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1};
// set same_color to 0 to show different colors for black and white keys
// when double color mode is activatied, set start_key to the number of semitones higher then C
const uint8_t start_key = 0, same_color = 0;
// set prelude color, red: 0, yellow: 43, lime: 85, cyan: 128, blue: 171, magenta: 213
const uint8_t prelude_hue = 213;
void setup()
{
  Serial.begin(19200);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(10);
  for (int i = 0; i < NUM_KEYS; i++)
  {
    remain_countdown[i] = 0;
  }
  tick = 0;
  queue_head = 0;
  queue_tail = 0;
  next_timer = micros() + time_step;
}

void loop()
{
  while (micros() < next_timer)
    ;
  FastLED.show();
  next_timer += time_step;
  tick++;
  if (recvlen = Serial.available())
  {
    Serial.readBytes(recvbuf, recvlen);
    for (int i = 0; i < recvlen; i++)
    {
      if (recvbuf[i] < NUM_KEYS)
      {
        // there is an issue when two same notes arrive in one prelude period
        // currently the latter note will reset the prelude time counter, showing weird behavior
        // if you uncomment the next line, the counter will not be reset, but the second counter will not be set as well
        // if (!prelude_countdown[recvbuf[i]])
        prelude_countdown[recvbuf[i]] = prelude_time;
        event_queue[queue_head] = tick + prelude_time;
        event_key[queue_head] = recvbuf[i];
        queue_head = (queue_head + 1) & 63;
      }
    }
  }
  while (queue_head != queue_tail)
  {
    if (event_queue[queue_tail] == tick)
    {
      remain_countdown[event_key[queue_tail]] = remain_time;
      queue_tail = (queue_tail + 1) & 63;
    }
    else
    {
      break;
    }
  }
  for (int i = 0; i < NUM_KEYS; i++)
  {
    if (prelude_countdown[i])
    {
      prelude_countdown[i]--;
      tmp_val = (uint16_t)255 * (uint16_t)prelude_countdown[i] / (uint16_t)prelude_time;
      if (prelude_countdown[i])
        if (same_color || key_type[(i + start_key) % 12])
          leds[i << 1] = CHSV(prelude_hue + tmp_val / (uint8_t)6, 255, (~tmp_val) >> 1);
        else
          leds[i << 1] = CHSV(prelude_hue - tmp_val / (uint8_t)6, 255, (~tmp_val) >> 1);
      else
        leds[i << 1] = CRGB(0, 0, 0);
    }
    if (remain_countdown[i])
    {
      remain_countdown[i]--;
      tmp_val = (int)255 * (int)remain_countdown[i] / (int)remain_time;
      leds[0 + (i << 1)] = CRGB(tmp_val, tmp_val, tmp_val);
    }
  }
}
