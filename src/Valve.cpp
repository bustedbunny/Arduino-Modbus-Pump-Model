#include <Arduino.h>
// Объявление класса клапана
class Valve
{
private:
    uint16_t curValue; // Переменная контроля степени открытия
    static const uint8_t MaxOpenValue = 100; // Максимальное значение степени открытия
public:
    Valve()
    {
        curValue = 0;
    }
    uint16_t OpenValue()
    {
        return curValue;
    }
    void OpenValveTick()  // Функция открытия (выполняется 2 раза в секунду, при подаче соответствующего сигнала)
    {
        if (curValue < MaxOpenValue)
            curValue += 5;
    }
    void CloseValveTick() // Функция закрытия
    {
        if (curValue > 0)
            curValue -= 5;
    }
};