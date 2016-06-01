
/*
https://www.arduino.cc/en/Reference/LiquidCrystal

LiquidCristal.cpp
*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "VTimer.h"
#include "definitions.h"
#include "LCD.h"


extern RCC_ClocksTypeDef MYCLOCKS;
extern short currentScaleValue, oldScaleValue, cementScaleValue, waterScaleValue;

size_t LCDwrite(uint8_t);
void LCDcommand(uint8_t);

//Row arrays for more friendly writting
char Row1[ROW_LENGHT], Row2[ROW_LENGHT], Row3[ROW_LENGHT], Row4[ROW_LENGHT];

/*
STM32F4_DISCOVERY_LOW_LEVEL Exported_Types
*/
extern uint32_t SystemCoreClock;
uint32_t OneMkSecCNT;

GPIO_TypeDef*  LCD_GPIO_PORT[LCDPins] = 
{LCDB0_GPIO_PORT, LCDB1_GPIO_PORT, LCDB2_GPIO_PORT, LCDB3_GPIO_PORT, LCDE_GPIO_PORT, LCDRS_GPIO_PORT};

const uint16_t LCD_GPIO_PIN[LCDPins] = 
{LCD_PIN_B0, LCD_PIN_B1, LCD_PIN_B2, LCD_PIN_B3, LCD_PIN_E, LCD_PIN_RS};

const uint32_t LCD_GPIO_CLK[LCDPins] = 
{LCDB0_GPIO_CLK, LCDB1_GPIO_CLK, LCDB2_GPIO_CLK, LCDB3_GPIO_CLK, LCDE_GPIO_CLK, LCDRS_GPIO_CLK};

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1 
//    S = 0; No shift 
//
// Note, however, that resetting the board doesn't reset the LCD, so one
// can't assume that its in that state when a sketch starts (and the
// LCD specific SET constructor is called).

/* static local variables */

static LCD_TypeDef _rw_pin; // LOW: write to LCD.  HIGH: read from LCD. - rw pin hard wired here
static LCD_TypeDef _rs_pin; // LOW: command.  HIGH: character.
static LCD_TypeDef _enable_pin; // activated by a HIGH pulse.
static LCD_TypeDef _data_pins[8];

static uint8_t _displayfunction;
static uint8_t _displaycontrol;
static uint8_t _displaymode;

static uint8_t _initialized=0;

static uint8_t _numlines;
static uint8_t _row_offsets[4];



/* SET functions depending on HW configuration */
/*
LCDLiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
init(0, rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

LCDLiquidCrystal(uint8_t rs, uint8_t enable,
uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
init(0, rs, 255, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

LCDLiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
init(1, rs, rw, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}
*/

void InitLCD(void)
{
    char Buffer [81];
    
    LCDSet(RS,Enb,B0,B1,B2,B3);
    LCDnoDisplay();
    LCDdisplay();
    delayMicroseconds(50000);
    SetVTimerValue(LCD_REFRESH_TIMER, T_1_S);
    
    //Write text
    sprintf(Row1, "Inert scale:  %4dkg", currentScaleValue);
    sprintf(Row2, "Cement scale: %4dkg", cementScaleValue);
    sprintf(Row3, "Water scale:  %4dkg", waterScaleValue);
    sprintf(Row4, "Sim. status:   %5s", "Ok");
    
    strcpy(Buffer, Row1);
    strcat(Buffer, Row3);
    strcat(Buffer, Row2);
    strcat(Buffer, Row4);
    Buffer[80] = '\0';
    
    LCDprint(Buffer);
}


void LCDSet(LCD_TypeDef rs,  LCD_TypeDef enable, LCD_TypeDef d0, LCD_TypeDef d1, LCD_TypeDef d2, LCD_TypeDef d3)
{
    OneMkSecCNT = SystemCoreClock/1000000UL;
    LCDinit(1, rs, NoPIN, enable, d0, d1, d2, d3, NoPIN, NoPIN, NoPIN, NoPIN);
}

void LCDinit(uint8_t fourbitmode, LCD_TypeDef rs, LCD_TypeDef rw, LCD_TypeDef enable,
             LCD_TypeDef d0, LCD_TypeDef d1, LCD_TypeDef d2, LCD_TypeDef d3,
             LCD_TypeDef d4, LCD_TypeDef d5, LCD_TypeDef d6, LCD_TypeDef d7)
{
    _rs_pin = rs;
    _rw_pin = rw;
    _enable_pin = enable;
    
    _data_pins[0] = d0;
    _data_pins[1] = d1;
    _data_pins[2] = d2;
    _data_pins[3] = d3; 
    _data_pins[4] = d4;
    _data_pins[5] = d5;
    _data_pins[6] = d6;
    _data_pins[7] = d7; 
    
    if (fourbitmode)
        _displayfunction = LCD_4BITMODE  | LCD_5x8DOTS;
    else 
        _displayfunction = LCD_8BITMODE  | LCD_5x8DOTS;
    
    LCDbegin(20, 4,LCD_5x8DOTS );  
}

void LCDbegin(uint8_t cols, uint8_t lines, uint8_t dotsize) 
{
    int i;
    
    if (lines > 1) {
        _displayfunction |= LCD_2LINE;
    } else {
        _displayfunction |= LCD_1LINE;
    }
    _numlines = lines;
    
    LCDsetRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);  
    
    /*
    // for some 1 line displays you can select a 10 pixel high font
    if ((dotsize != LCD_5x8DOTS) && (lines == 1)) {
    _displayfunction |= LCD_5x10DOTS;
}
    */
    digitalWrite(_rs_pin, LOW); //preset outputs
    digitalWrite(_enable_pin, LOW);
    
    pinMode(_rs_pin, OUTPUT);
    // we can save 1 pin by not using RW. Indicate by passing NoPIN instead of pin#
    if (_rw_pin != NoPIN) { 
        pinMode(_rw_pin, OUTPUT);
    }
    pinMode(_enable_pin, OUTPUT);
    
    // Do these once, instead of every time a character is drawn for speed reasons.
    for (i=0; i<((_displayfunction & LCD_8BITMODE) ? 8 : 4); ++i)
    {
        pinMode(_data_pins[i], OUTPUT);
    } 
    
    // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    // according to datasheet, we need at least 40ms after power rises above 2.7V before 
    // sending   commands. Arduino can turn on way before 4.5V so we'll wait 50   
    
    delayMicroseconds(50000); 
    // Now we pull both RS and R/W low to begin commands
    digitalWrite(_rs_pin, LOW);
    digitalWrite(_enable_pin, LOW);
    if (_rw_pin != NoPIN) { 
        digitalWrite(_rw_pin, LOW);
    }
    
    //put the LCD into 4 bit or 8 bit mode
    if (! (_displayfunction & LCD_8BITMODE)) {
        // this is according to the hitachi HD44780 datasheet
        // figure 24, pg 46
        
        // we start in 8bit mode, try to set 4 bit mode
        LCDwrite4bits(0x03);
        delayMicroseconds(4500); // wait min 4.1ms
        
        // second try
        LCDwrite4bits(0x03);
        delayMicroseconds(4500); // wait min 4.5 ms
        
        // third go!
        LCDwrite4bits(0x03); 
        delayMicroseconds(150);
        
        // finally, set to 4-bit interface
        LCDwrite4bits(0x02); 
        delayMicroseconds(150);
    } else {
        // this is according to the hitachi HD44780 datasheet
        // page 45 figure 23
        
        // Send function set command sequence
        LCDcommand(LCD_FUNCTIONSET | _displayfunction);
        delayMicroseconds(4500);  // wait more than 4.1ms
        
        // second try
        LCDcommand(LCD_FUNCTIONSET | _displayfunction);
        delayMicroseconds(150);
        
        // third go
        LCDcommand(LCD_FUNCTIONSET | _displayfunction);
    }
    
    // finally, set # lines, font size, etc.
    LCDcommand(LCD_FUNCTIONSET | _displayfunction);  
    
    // turn the display on with no cursor or blinking default
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
    LCDdisplay();
    
    // clear it off
    LCDclear();
    
    // Initialize to default text direction (for romance languages)
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    // set the entry mode
    LCDcommand(LCD_ENTRYMODESET | _displaymode);
    
}

void LCDsetRowOffsets(int row0, int row1, int row2, int row3)
{
    _row_offsets[0] = row0;
    _row_offsets[1] = row1;
    _row_offsets[2] = row2;
    _row_offsets[3] = row3;
}

/********** high level commands, for the user! */
void LCDclear()
{
    LCDcommand(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
    delayMicroseconds(2000);  // this command takes a long time!
}

void LCDhome()
{
    LCDcommand(LCD_RETURNHOME);  // set cursor position to zero
    delayMicroseconds(2000);  // this command takes a long time!
}

void LCDsetCursor(uint8_t col, uint8_t row)
{
    const size_t max_lines = sizeof(_row_offsets) / sizeof(*_row_offsets);
    if ( row >= max_lines ) {
        row = max_lines - 1;    // we count rows starting w/0
    }
    if ( row >= _numlines ) {
        row = _numlines - 1;    // we count rows starting w/0
    }
    
    LCDcommand(LCD_SETDDRAMADDR | (col + _row_offsets[row]));
}

// Turn the display on/off (quickly)
void LCDnoDisplay() {
    _displaycontrol &= ~LCD_DISPLAYON;
    LCDcommand(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LCDdisplay() {
    _displaycontrol |= LCD_DISPLAYON;
    LCDcommand(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LCDnoCursor() {
    _displaycontrol &= ~LCD_CURSORON;
    LCDcommand(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LCDcursor() {
    _displaycontrol |= LCD_CURSORON;
    LCDcommand(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LCDnoBlink() {
    _displaycontrol &= ~LCD_BLINKON;
    LCDcommand(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LCDblink() {
    _displaycontrol |= LCD_BLINKON;
    LCDcommand(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LCDscrollDisplayLeft(void) {
    LCDcommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LCDscrollDisplayRight(void) {
    LCDcommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LCDleftToRight(void) {
    _displaymode |= LCD_ENTRYLEFT;
    LCDcommand(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LCDrightToLeft(void) {
    _displaymode &= ~LCD_ENTRYLEFT;
    LCDcommand(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LCDautoscroll(void) {
    _displaymode |= LCD_ENTRYSHIFTINCREMENT;
    LCDcommand(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LCDnoAutoscroll(void) {
    _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    LCDcommand(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LCDcreateChar(uint8_t location, uint8_t charmap[]) 
{
    int i;
    location &= 0x7; // we only have 8 locations 0-7
    LCDcommand(LCD_SETCGRAMADDR | (location << 3));
    for (i=0; i<8; i++) {
        LCDwrite(charmap[i]);
    }
}

/*********** mid level commands, for sending data/cmds */

inline void LCDcommand(uint8_t value) {
    LCDsend(value, LOW);
}

inline size_t LCDwrite(uint8_t value) {
    LCDsend(value, HIGH);
    return 1; // assume sucess
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void LCDsend(uint8_t value, uint8_t mode) {
    digitalWrite(_rs_pin, mode);
    
    // if there is a RW pin indicated, set it low to Write
    if (_rw_pin != NoPIN) { 
        digitalWrite(_rw_pin, LOW);
    }
    
    if (_displayfunction & LCD_8BITMODE) {
        LCDwrite8bits(value); 
    } else {
        LCDwrite4bits(value>>4);
        LCDwrite4bits(value);
    }
}

void LCDpulseEnable(void) {
    digitalWrite(_enable_pin, LOW);
    delayMicroseconds(1);    
    digitalWrite(_enable_pin, HIGH);
    delayMicroseconds(1);    // enable pulse must be >450ns
    digitalWrite(_enable_pin, LOW);
    delayMicroseconds(100);   // commands need > 37us to settle
}

void LCDwrite4bits(uint8_t value) {
    int i;
    for (i = 0; i < 4; i++) {
        digitalWrite(_data_pins[i], (value >> i) & 0x01);
    }
    
    LCDpulseEnable();
}

void LCDwrite8bits(uint8_t value) {
    int i;
    for (i = 0; i < 8; i++) {
        digitalWrite(_data_pins[i], (value >> i) & 0x01);
    }
    
    LCDpulseEnable();
}

/**
* @brief  Configures GPIO input or output pins according to LCD pins connected.
* @param  pin: Specifies the PINx to be configured. 
*   This parameter can be one of following parameters:
*     @arg B1, B0, B2, B3, RS, E
*     
* @retval None
*/

void    pinMode(LCD_TypeDef pin, uint8_t mode)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    if ( mode == OUTPUT ){
        /* Enable the GPIO_PINx Clock */
        RCC_AHB1PeriphClockCmd(LCD_GPIO_CLK[pin], ENABLE);
        
        /* Configure the GPIO_PINx pin */
        GPIO_InitStructure.GPIO_Pin = LCD_GPIO_PIN[pin];
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        if ( pin == Enb ) 
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; // GPIO_PuPd_NOPULL; 
        else 
            GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_NOPULL; 
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(LCD_GPIO_PORT[pin], &GPIO_InitStructure);
        
    } else if (mode == INPUT && pin == B0) {
        RCC_AHB1PeriphClockCmd(LCD_GPIO_CLK[pin], ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
        
        /* Configure Button pin as input */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = LCD_GPIO_CLK[pin];
        GPIO_Init(LCD_GPIO_PORT[pin], &GPIO_InitStructure);
        
    } else 
        ;
    
}


uint8_t  digitalRead(LCD_TypeDef pin)
{
    return 0; // not used
}


void  digitalWrite(LCD_TypeDef pin, uint8_t mode)
{
    if (mode == LOW ){
        LCD_GPIO_PORT[pin]->BSRRH = LCD_GPIO_PIN[pin];
    } else if (mode == HIGH ){
        LCD_GPIO_PORT[pin]->BSRRL = LCD_GPIO_PIN[pin];
    } else 
        ;
    
}

void delayMicroseconds(unsigned long mksec)
{
    
    //    unsigned rpt;
    //    char m=0;
    //    unsigned cntr=9;
    //    /* normal */
    //    while( mksec--){ // 5+4 clk init
    //      for ( rpt = cntr; rpt; rpt--);  // 6 clk cyclle
    //    }
    unsigned int i, cycles;
    
    //      1, sec                           ----> 100 000 000, cycles
    //      1, micro sec (0.000001, sec)     ----> X, cycles
    //      X, cycles = (100 000 000, cycles * 0.000001, s) / 1, s
    unsigned int cyclesPer1microSecond = (unsigned int)(0.000001 * MYCLOCKS.SYSCLK_Frequency);
    
    cycles = cyclesPer1microSecond * mksec;
    
    for(i = 0; i < cycles; i++)
    {
        //do nothing;
    }
}

uint16_t LCDStrWrite(const uint8_t *buffer, uint16_t size)
{
    uint16_t n = 0;
    while (size--) {
        if (LCDwrite((char)*buffer++)) n++;
        else break;
    }
    return n;
}


void LCDTask(void)
{
    char buffer [7];
    
    sprintf(buffer, "%4dkg", currentScaleValue);
    LCDsetCursor(14, 0); 
    LCDprint(buffer);
    
    sprintf(buffer, "%4dkg", cementScaleValue);
    LCDsetCursor(14, 1); 
    LCDprint(buffer);
    
    sprintf(buffer, "%4dkg", waterScaleValue);
    LCDsetCursor(14, 2); 
    LCDprint(buffer);
    
//    if()
//    sprintf(buffer, "Sim. status:   %5s", "Ok");
//    LCDsetCursor(14, 3); 
//    LCDprint(buffer);
    
}

uint16_t LCDprint(char* s)
{
    uint16_t sLenght = (uint16_t)strlen(s);
    return LCDStrWrite((const uint8_t *)s, sLenght);
}

