#pragma once
#ifndef GAME_H
#define GAME_H

#include "types.h"
#include "libs.h"


class Match;
class Menu;

enum class Language;


///////////////////////////////////////////////////////
//  base raspberry pi example boilerplate :DDDD      //
///////////////////////////////////////////////////////

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS 17
#define PIN_SCK 18
#define PIN_MOSI 19

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

//#include "blink.pio.h"


// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 4
#define UART_RX_PIN 5

enum class GameModes;
struct MatchSettings;


namespace tetrismatrix
{


    extern bool debug;

    extern Match *current_match;
    extern int frames;

    extern Hub75 hub75;
    extern PicoGraphics_PenRGB888 graphics;

    extern bool vertical;

    extern GameModes selected_game_mode;
    extern MatchSettings selected_match_settings;
    extern Language past_language;



    extern std::stack<Menu*> menus;


    struct GameSettings
    {
        OptionValue volume_sfx = 100;
        OptionValue volume_music = 100;
        OptionValue current_language;
    };
    extern GameSettings settings;


    
    void initGame();
    void updateGame();

    int screenWidth();
    int screenHeight();

    Menu* createMenu(Menus menu_type);
    void popMenu();
};

    // Callback for the dma interrupt (required)
    static void __isr dma_complete() 
    {
        tetrismatrix::hub75.dma_complete();
    }




#endif