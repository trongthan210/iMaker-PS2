/*******************************************************************************
  Thư viện được xây dựng lại và phát triển bởi iMaker
  Website: https://imaker.vn
  Zalo hỗ trợ kỹ thuật: 0971113732

  Hỗ trợ tay cầm PS2 tại Shop. Loại tay cầm mới chuyển khắc phục lỗi trùng tín hiệu thu phát.
  Loại tay cầm này chuyên sử dụng cho các Robot tránh bị trùng mạch thu phát giữa nhiều PS2 sử dụng cùng lúc.
********************************************************************************/

#include <iMakerPS2.h>

#include <avr/pgmspace.h>
typedef const __FlashStringHelper * FlashStr;
typedef const byte* PGM_BYTES_P;
#define PSTR_TO_F(s) reinterpret_cast<const __FlashStringHelper *> (s)

// Các chân cấu hình từ mạch chuyển đổi vào Arduino
const byte PIN_PS2_ATT = 10;
const byte PIN_PS2_CMD = 11;
const byte PIN_PS2_DAT = 12;
const byte PIN_PS2_CLK = 13;

#define ERR 255
#define SELECT 0
#define START 3
#define UP 4
#define RIGHT 5
#define DOWN 6
#define LEFT 7
#define L2 8
#define R2 9
#define L1 10
#define R1 11
#define TRIANGLE 12 //TAM GIÁC
#define CIRCLE 13   //TRÒN
#define CROSS 14    //X
#define SQUARE 15   //VUÔNG

byte psxButtonToIndex (PsxButtons psxButtons) {
  byte i;

  for (i = 0; i < PSX_BUTTONS_NO; ++i) {
    if (psxButtons & 0x01) {
      break;
    }

    psxButtons >>= 1U;
  }
  return i;
}

void controller(byte data)
{
  switch (data) {
    case ERR:// Hàm khi mà không nhấn nút gì
      //Serial.println(F("ERR"));
      break;
    case SELECT:
      Serial.println(F("SELECT"));
      break;
    case START:
      Serial.println(F("START"));
      break;
    case UP:
      Serial.println(F("UP"));
      break;
    case RIGHT:
      Serial.println(F("RIGHT"));
      break;
    case DOWN:
      Serial.println(F("DOWN"));
      break;
    case LEFT:
      Serial.println(F("LEFT"));
      break;
    case L2:
      Serial.println(F("L2"));
      break;
    case R2:
      Serial.println(F("R2"));
      break;
    case L1:
      Serial.println(F("L1"));
      break;
    case R1:
      Serial.println(F("R1"));
      break;
    case TRIANGLE:
      Serial.println(F("TRIANGLE"));
      break;
    case CIRCLE:
      Serial.println(F("CIRCLE"));
      break;
    case CROSS:
      Serial.println(F("CROSS"));
      break;
    case SQUARE:
      Serial.println(F("SQUARE"));
      break;
    default:;
  }
}
void dumpButtons (PsxButtons psxButtons) {
  static PsxButtons lastB = 0;
  if (psxButtons != lastB) {
    for (byte i = 0; i < PSX_BUTTONS_NO; ++i) {
      byte b = psxButtonToIndex (psxButtons);
      if (b < PSX_BUTTONS_NO) {
        controller(b);
      }
      psxButtons &= ~(1 << b);
      if (psxButtons != 0) {
      }
    }
    lastB = psxButtons;
  }
  else controller(255);
}

void dumpAnalog (const char *str, const byte x, const byte y) {
  Serial.print (str);
  Serial.print (F(" analog: x = "));
  Serial.print (x);
  Serial.print (F(", y = "));
  Serial.println (y);
}

const char ctrlTypeUnknown[] PROGMEM = "Unknown";
const char ctrlTypeDualShock[] PROGMEM = "Dual Shock";
const char ctrlTypeDsWireless[] PROGMEM = "Dual Shock Wireless";
const char ctrlTypeGuitHero[] PROGMEM = "Guitar Hero";
const char ctrlTypeOutOfBounds[] PROGMEM = "(Out of bounds)";

const char* const controllerTypeStrings[PSCTRL_MAX + 1] PROGMEM = {
  ctrlTypeUnknown,
  ctrlTypeDualShock,
  ctrlTypeDsWireless,
  ctrlTypeGuitHero,
  ctrlTypeOutOfBounds
};
PsxControllerBitBang<PIN_PS2_ATT, PIN_PS2_CMD, PIN_PS2_DAT, PIN_PS2_CLK> psx;

boolean haveController = false;

void setup () {
  Serial.begin (115200);
  Serial.println (F("Ready!"));
}

void loop () {
  static byte slx, sly, srx, sry;
  if (!haveController) {
    if (psx.begin ()) {
      Serial.println (F("Controller found!"));
      delay (300);
      if (!psx.enterConfigMode ()) {
        Serial.println (F("Cannot enter config mode"));
      } else {
        PsxControllerType ctype = psx.getControllerType ();
        PGM_BYTES_P cname = reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(controllerTypeStrings[ctype < PSCTRL_MAX ? static_cast<byte> (ctype) : PSCTRL_MAX])));

        Serial.print (F("Controller Type is: "));
        Serial.println (PSTR_TO_F (cname));

        if (!psx.enableAnalogSticks ()) {
          Serial.println (F("Cannot enable analog sticks"));
        }
        if (!psx.enableAnalogButtons ()) {
          Serial.println (F("Cannot enable analog buttons"));
        }
        if (!psx.exitConfigMode ()) {
          Serial.println (F("Cannot exit config mode"));
        }
      }

      haveController = true;
    }
  } else {
    if (!psx.read ()) {
      Serial.println (F("Controller lost :("));
      haveController = false;
    } else {
      dumpButtons(psx.getButtonWord());

      byte lx, ly;
      psx.getLeftAnalog (lx, ly);
      if (lx != slx || ly != sly) {
        dumpAnalog ("Left", lx, ly);
        slx = lx;
        sly = ly;
      }

      byte rx, ry;
      psx.getRightAnalog (rx, ry);
      if (rx != srx || ry != sry) {
        dumpAnalog ("Right", rx, ry);
        srx = rx;
        sry = ry;
      }
    }
  }
  delay (1000 / 60);
}
//Khách hàng muốn lấy giá trị Joystick thì đọc biến srx,sry và slx,sly.
//Delay được tính toán hợp lý bạn có nhu cầu thay đổi có thể thử.
