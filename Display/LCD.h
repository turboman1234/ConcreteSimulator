// LCD.h
#ifndef LCD_h 
#define LCD_h

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// general flags
#define LOW 0
#define HIGH 1
#define OUTPUT 33
#define INPUT 44

//constants
#define ROW_LENGHT              21

/* STM32F4_DISCOVERY_LOW_LEVEL Exported_Types
    */
    
typedef enum 
{
  B0 = 0,
  B1 = 1,
  B2 = 2,
  B3 = 3,
  Enb = 4,
  RS = 5,
  NoPIN = 6
} LCD_TypeDef;


void InitLCD(void);
void LCDSet(LCD_TypeDef rs, LCD_TypeDef enable, LCD_TypeDef d0, LCD_TypeDef d1, LCD_TypeDef d2, LCD_TypeDef d3);

void LCDinit(uint8_t fourbitmode, LCD_TypeDef rs, LCD_TypeDef rw, LCD_TypeDef enable,
          LCD_TypeDef d0, LCD_TypeDef d1, LCD_TypeDef d2, LCD_TypeDef d3,
          LCD_TypeDef d4, LCD_TypeDef d5, LCD_TypeDef d6, LCD_TypeDef d7);
  
void LCDbegin(uint8_t cols, uint8_t rows, uint8_t charsize );

void LCDclear();
void LCDhome();

void LCDnoDisplay();
void LCDdisplay();
void LCDnoBlink();
void LCDblink();
void LCDnoCursor();
void LCDcursor();
void LCDscrollDisplayLeft();
void LCDscrollDisplayRight();
void LCDleftToRight();
void LCDrightToLeft();
void LCDautoscroll();
void LCDnoAutoscroll();

void LCDsetRowOffsets(int row1, int row2, int row3, int row4);
void LCDcreateChar(uint8_t, uint8_t[]);
void LCDsetCursor(uint8_t, uint8_t); 

void LCDsend(uint8_t, uint8_t);
void LCDwrite4bits(uint8_t);
void LCDwrite8bits(uint8_t);
void LCDpulseEnable();

void LCDTask(void);
uint16_t LCDprint(char* s);
uint16_t LCDStrWrite(const uint8_t *buffer, uint16_t size);


void pinMode(LCD_TypeDef pin, uint8_t mode);
void digitalWrite(LCD_TypeDef pin, uint8_t mode);
void delayMicroseconds(unsigned long mksec);

/**  STM32F4_DISCOVERY_LOW_LEVEL  LCD LINES
  * 
  */
#define LCDDataLines                     4
#define LCDPins                          6

#define LCD_PIN_B0                       GPIO_Pin_13
#define LCDB0_GPIO_PORT                  GPIOB
#define LCDB0_GPIO_CLK                   RCC_AHB1Periph_GPIOB  
  
#define LCD_PIN_B1                       GPIO_Pin_12
#define LCDB1_GPIO_PORT                  GPIOB
#define LCDB1_GPIO_CLK                   RCC_AHB1Periph_GPIOB  
  
#define LCD_PIN_B2                       GPIO_Pin_6
#define LCDB2_GPIO_PORT                  GPIOA
#define LCDB2_GPIO_CLK                   RCC_AHB1Periph_GPIOA  
  
#define LCD_PIN_B3                       GPIO_Pin_7
#define LCDB3_GPIO_PORT                  GPIOA
#define LCDB3_GPIO_CLK                   RCC_AHB1Periph_GPIOA  
  
#define LCD_PIN_RS                       GPIO_Pin_15
#define LCDRS_GPIO_PORT                  GPIOB
#define LCDRS_GPIO_CLK                   RCC_AHB1Periph_GPIOB  
  
#define LCD_PIN_E                        GPIO_Pin_14
#define LCDE_GPIO_PORT                   GPIOB
#define LCDE_GPIO_CLK                    RCC_AHB1Periph_GPIOB  

/**
  * 
*/ 


#endif

