#include <IRremote.h>

const int receiverPin = 11;
const int ledPin = 5;  // Пин, к которому подключен светодиод
bool power = false;

IRrecv irrecv(receiverPin);

decode_results results;

int initialBlinkInterval;  // Переменная для хранения исходного значения интервала мигания
int initialFadeInterval;   // Переменная для хранения исходного значения интервала изменения яркости

// Интервалы между миганиями и изменениями яркости (мс)
unsigned long blinkInterval = 50;
unsigned long fadeInterval = 10;

void setup() {
  // Инициализация
  initialBlinkInterval = blinkInterval;  // Сохраняем исходное значение интервала мигания
  initialFadeInterval = fadeInterval;    // Сохраняем исходное значение интервала изменения яркости
  Serial.begin(9600);
  irrecv.enableIRIn();      // Инициализация приемника ИК
  pinMode(ledPin, OUTPUT);  // Устанавливаем пин светодиода как выход
}

int mode;  // переключение режимов

const unsigned long on = 0xF7C03F;               //F7C03F - power
const unsigned long red = 0xF720DF;              // F720DF - red
const unsigned long green = 0xF7A05F;            // F7A05F - green
const unsigned long blue = 0xF7609F;             // F7609F - blue
const unsigned long white = 0xF7E01F;            // F7E01F - white
const unsigned long red_plus_int = 0xF710EF;     // F710EF - red увелечение интервала
const unsigned long red_minus_speed = 0xF730CF;  // F710EF - red уменьшение интервала

// Объявляем переменные для эффекта затухания
int redBrightness = 0;  // Начальная яркость светодиода
int fadeAmount = 5;     // Шаг изменения яркости
unsigned long previousMillis = 0;

void loop() {
  if (irrecv.decode(&results)) {
    Serial.print("Received IR code: ");
    Serial.println(results.value, HEX);

    switch (results.value) {
      case on:  // Если получен сигнал для включения/выключения
        Serial.println("Red");
        powerBtn();
        break;

      case red:  // Если получен сигнал для установки красного цвета
        Serial.println("Red");
        if (power)
          mode = 1;
        break;

      case green:  // Если получен сигнал для установки зеленого цвета
        Serial.println("Green");
        if (power)
          mode = 2;
        // Ваш код для установки зеленого цвета
        break;

      case blue:  // Если получен сигнал для установки синего цвета
        Serial.println("Blue");
        if (power)
          mode = 3;
        // Ваш код для установки синего цвета
        break;

      case white:  // Если получен сигнал для установки белого цвета
        Serial.println("White");
        if (power)
          mode = 4;
        // Ваш код для установки синего цвета
        break;

      case red_plus_int:  // Если получен сигнал для увеличения скорости интвервала мерцания
        Serial.println("Add");
        if (power && mode == 1)
          increaseBlinkInterval(power);
        // Ваш код для установки синего цвета
        break;

      case red_minus_speed:  // Если получен сигнал для уменьшения скорости интвервала мерцания
        Serial.println("Minus");
        if (power && mode == 1)
          decreaseBlinkInterval(power);
        // Ваш код для установки синего цвета
        break;

      default:
        // Ничего не делаем для остальных сигналов
        break;
    }

    irrecv.resume();  // Продолжаем прием данных
    // Serial.println(power, "all");
  }

  // redBtn(true); // постоянное включение красной кнопки
  // greenBtn(true); // постоянное включение зеленой кнопки
  // blueBtn(true);  // постоянное включение синей кнопки

  switch (mode) {
    case 1:
      redBtn(power);
      break;
    case 2:
      greenBtn(power);
      break;
    case 3:
      blueBtn(power);
      break;
    case 4:
      lightBtn(power);
      break;
  }
}

// Функции анимции
// --------------------------------------------
void powerBtn() {
  power = !power;
  if (power) {
    Serial.println("On");
    digitalWrite(ledPin, HIGH);
  } else {
    Serial.println("Off");
    digitalWrite(ledPin, LOW);
  }
}

// красная кнопка - настраиваемые мигания
void redBtn(bool power) {
  unsigned long currentMillis = millis();

  if (power) {
    // Обработка затухания
    if (currentMillis - previousMillis >= fadeInterval) {
      previousMillis = currentMillis;
      redBrightness += fadeAmount;
      if (redBrightness <= 0 || redBrightness >= 255) {
        fadeAmount = -fadeAmount;
      }
      analogWrite(ledPin, redBrightness);
    }

    // Обработка мигания
    if (currentMillis - previousMillis >= blinkInterval) {
      previousMillis = currentMillis;
      digitalWrite(ledPin, !digitalRead(ledPin));  // Мигаем светодиодом
    }
  } else {
    // Выключаем светодиод, если свет выключен
    digitalWrite(ledPin, LOW);
  }
}


// Зеленая кнопка - рандомные мигания
void greenBtn(bool power) {
  if (power) {
    // Генерируем случайное значение для скорости мигания в диапазоне от 100 до 500 мс
    unsigned long blinkInterval = random(100, 500);

    // Генерируем случайное значение для времени между изменениями яркости в диапазоне от 10 до 50 мс
    unsigned long fadeInterval = random(10, 50);

    // Выполняем мигание и затухание светодиода с полученными интервалами
    unsigned long currentMillis = millis();
    static unsigned long previousMillis = currentMillis;
    static unsigned long previousFadeMillis = currentMillis;

    if (currentMillis - previousMillis >= blinkInterval) {
      previousMillis = currentMillis;
      digitalWrite(ledPin, !digitalRead(ledPin));  // Мигаем светодиодом
    }

    if (currentMillis - previousFadeMillis >= fadeInterval) {
      previousFadeMillis = currentMillis;
      redBrightness += fadeAmount;
      if (redBrightness <= 0 || redBrightness >= 255) {
        fadeAmount = -fadeAmount;
      }
      analogWrite(ledPin, redBrightness);
    }
  } else {
    // Выключаем светодиод, если свет выключен
    digitalWrite(ledPin, LOW);
  }
}

// Синяя кнопка - рандом яркость и интервал мерцания
void blueBtn(bool power) {
  if (power) {
    // Задаем случайную начальную яркость
    int brightness = random(0, 255);

    // Интервал времени между изменениями яркости (мс)
    unsigned long interval = random(20, 100);

    // Количество изменений яркости
    int steps = random(10, 30);

    // Изменяем яркость постепенно
    for (int i = 0; i < steps; i++) {
      // Изменяем яркость на случайное значение в пределах [-50, 50]
      brightness += random(-50, 50);
      // Ограничиваем яркость в пределах [0, 255]
      brightness = constrain(brightness, 0, 255);
      // Устанавливаем новую яркость
      analogWrite(ledPin, brightness);
      // Задержка перед следующим изменением
      delay(interval);
    }
  } else {
    // Выключаем светодиод, если свет выключен
    digitalWrite(ledPin, LOW);
  }
}

// Белая кнопка - просто включен
void lightBtn(bool power) {
  if (power) {
    digitalWrite(ledPin, HIGH);
  } else {
    // Выключаем светодиод, если свет выключен
    digitalWrite(ledPin, LOW);
  }
}

void increaseBlinkInterval(bool power) {
  if (power) {
    // Проверяем, не превышает ли новое значение 10-ное значение исходного интервала мигания
    if (blinkInterval < initialBlinkInterval * 10) {
      // Увеличиваем интервал мигания
      blinkInterval += 1;
    }
    // Проверяем, не превышает ли новое значение 10-ное значение исходного интервала изменения яркости
    if (fadeInterval < initialFadeInterval * 10) {
      // Увеличиваем интервал изменения яркости
      fadeInterval += 1;
    }
  }
}

void decreaseBlinkInterval(bool power) {
  if (power) {
    // Уменьшаем интервал мигания и интервал изменения яркости
    blinkInterval -= 1;
    fadeInterval -= 1;
    // Ограничиваем значения интервалов в диапазоне от 1 до исходного значения, но не ниже 1
    blinkInterval = constrain(blinkInterval, 1, initialBlinkInterval);
    fadeInterval = constrain(fadeInterval, 1, initialFadeInterval);
  }
}