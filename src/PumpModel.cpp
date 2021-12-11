#include <Arduino.h>
#include <Valve.cpp>

enum ChangeType
{
  None,
  Open,
  Close
};

static ChangeType GetType(int i)
{
  switch (i)
  {
  case 11:
    return ChangeType::Open;
  case 10:
    return ChangeType::Close;
  default:
    return ChangeType::None;
  }
}

class PumpModel
{
private:
  Valve outValve;
  Valve inValve;

  // Поля для симуляции процесса
  bool Debug = false;
  uint16_t _targetValue;
  unsigned long _prevTimer;
  static const unsigned long _interval = 30000;
  ////////////////////////////////////////////////

  uint16_t curValue;
  uint16_t *buffer;

  static const int amountOfValves = 2;

  Valve &GetValve(int ind)
  {
    switch (ind)
    {
    case 0:
      return outValve;
    default:
      return inValve;
    }
  }
// Функция для изменения значения резервуара с избежанием целочисленного переполнения
  void ChangeCurSafe(int32_t value)
  {
    if (value > 0)
    {
      if (curValue + value > 65535)
      {
        curValue = 65535;
        return;
      }
    }
    else
    {
      if (curValue <= value)
      {
        curValue = 0;
        return;
      }
    }
    curValue += value;
  }
public:
  PumpModel(uint16_t *array)
  {
    if (array)
    {
      buffer = array;
    }
    curValue = 10000;
  };
  // Вызывает функции открытия/закрытия в клапанах
  void MoveValve(int ind, ChangeType type)
  {
    if (type == ChangeType::Open)
    {
      GetValve(ind).OpenValveTick();
    }
    else if (type == ChangeType::Close)
    {
      GetValve(ind).CloseValveTick();
    }
  }
  // Добавляет значение указанное в буфере и устанавливает его в 0.
  void AddWater()
  {
    if (buffer[8] > 0)
    {
      ChangeCurSafe(buffer[8]);
      buffer[8] = 0;
    }
  }
  // Текущее значение клапана (0 полностью закрыто, >0 степень открытия)
  uint16_t GetValveConc(int ind)
  {
    return GetValve(ind).OpenValue();
  }
  //По умолчанию выполняется 2 раза в секунду
  void Tick()
  {
    // Изменение значений клапанов
    for (int i = 0; i < amountOfValves; i++)
    {
      MoveValve(i, GetType(buffer[i]));
      // Запись текущего значения клапанов в буфер
      buffer[i + amountOfValves] = GetValveConc(i);
    }
    // Изменение значения воды на разность клапанов
    ChangeCurSafe((int32_t)inValve.OpenValue() - (int32_t)outValve.OpenValue());

    // Случайное изменение значений воды для тестирования
    if (buffer[4])
    {
      // Изменение целевого значения
      if (millis() - _prevTimer >= _interval)
      {
        _prevTimer = millis();
        _targetValue = random(0, 65535);
        buffer[5] = _targetValue;
      }
      // Изменение значения воды приближая его к целевому
      if ((int32_t)curValue - _targetValue > 0)
      {
        ChangeCurSafe(random(50, 100) * -1);
      }
      else
      {
        ChangeCurSafe(random(50, 100));
      }
    }

    // Запись текущего значения воды в буфер
    buffer[9] = curValue;
  }

  uint16_t GetValue()
  {
    return curValue;
  }
};
