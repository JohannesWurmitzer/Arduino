#include <avr/wdt.h>
uint8_t mcusr_mirror __attribute__ ((section (".noinit")));
void get_mcusr(void) \
  __attribute__((naked)) \
  __attribute__((section(".init3")));
void get_mcusr(void)
{
  mcusr_mirror = MCUSR;
  MCUSR = 0;
  wdt_disable();
}
void setup() {
  // put your setup code here, to run once:
  wdt_enable(WDTO_4S);
  Serial.begin(115200);
  Serial.print("MCUSR: ");
  Serial.println(MCUSR);
  Serial.print("MCUSRmirror: ");
  Serial.println(mcusr_mirror);
  MCUSR = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(millis());
  delay(500);
}
