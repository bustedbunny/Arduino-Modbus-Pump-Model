#include <Arduino.h>
#include "ModbusRtu.h"
#include <PumpModel.cpp>
// Буфер обмена с Modbus Master
uint16_t au16data[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// Объявление объекта Modbus Slave
Modbus slave(1, Serial, 0);
int8_t state = 0;
unsigned long tempus = 0;
// Объявление объекта модели насосной станции
PumpModel pump(au16data);
void setup()
{
  Serial.begin(115200); // Инициализация Serial порта
  slave.start();
}
// Переменные для создания интервала
unsigned long curMil = millis();
unsigned long prevMil = 0;
static unsigned long interval = 500;
/////////////////////////////////////
void ProcessData() // Функция вызываемая при успешном получении сигнала от Master
{
  pump.AddWater();
  au16data[9] = pump.GetValue();
  if (au16data[7] > 0)
  {
    interval = au16data[7];
  }
}
void loop()
{
  curMil = millis();
  state = slave.poll(au16data, 16);
  if (state > 4) // Если сигнал от Master без ошибок
  {
    tempus = millis() + 500;
    digitalWrite(LED_BUILTIN, HIGH); // Включим встроенный светодиод 500 мс.
    ProcessData();
  }
  if (curMil > tempus)
  {
    digitalWrite(LED_BUILTIN, LOW); // Выключим светодиод спустя 500 мс.
  }

  if (curMil - prevMil >= interval) // Таймер на заданный interval
  {
    prevMil = curMil;
    pump.Tick();
  }
}
