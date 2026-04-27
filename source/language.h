#include "libs.h"

enum class Language
{
    EN,
    ES,
    SIZE
};

enum class Texts
{
    START,
    OPTIONS,
    SCORES,
    VOLUME,
    SEARCHING,
    CONTROLLERS,
    CONTROLLER,
    CONNECTED,
    RESUME,
    EXIT,
    PAUSE,
    ZEN,
    MARATHON,
    VERSUS,
    COOP,
    DUAL_BOARD,
    GAME_MODE,
    VOLUME_SFX,
    VOLUME_MUSIC,
    UPCOMING_PIECES,
    GAME_SCALE,
    BOARD_WIDTH,
    BOARD_HEIGHT,
    VELOCITY,
    LOCK_DELAY,
    MAX_VELOCITY,
    GARBAGE_MULTIPLIER,
    GARBAGE_BLOCKING,
    LANGUAGE,
    SETTINGS,
    YES,
    NO,
    ENGLISH,
    SPANISH
};
using enum Texts;

using TranslationMap = std::unordered_map<Texts, std::string>;
using Translations = std::unordered_map<Language, TranslationMap>;

inline Translations translations = {
    {
        Language::EN, 
        {
            {START, "START"},
            {OPTIONS, "OPTIONS"},
            {SCORES, "SCORES"},
            {VOLUME, "VOLUME"},
            {SEARCHING, "SEARCHING"},
            {CONTROLLERS, "CONTROLLERS"},
            {CONTROLLER, "CONTROLLER"},
            {CONNECTED, "CONNECTED"},
            {RESUME, "RESUME"},
            {EXIT, "EXIT"},
            {PAUSE, "PAUSE"},
            {ZEN, "ZEN"},
            {MARATHON, "MARATHON"},
            {VERSUS, "VERSUS"},
            {COOP, "COOP"},
            {DUAL_BOARD, "DUAL BOARD"},
            {GAME_MODE, "GAME MODE"},
            {VOLUME_SFX, "VOLUME SFX"},
            {VOLUME_MUSIC, "VOLUME MUSIC"},
            {UPCOMING_PIECES,"UPCOMING PIECES"},
            {GAME_SCALE, "GAME SCALE"},
            {BOARD_WIDTH, "BOARD WIDTH"},
            {BOARD_HEIGHT, "BOARD HEIGHT"},
            {VELOCITY, "VELOCITY"},
            {LOCK_DELAY, "LOCK DELAY"},
            {MAX_VELOCITY, "MAX VELOCITY"},
            {GARBAGE_MULTIPLIER, "GARBAGE MULTIPLIER"},
            {GARBAGE_BLOCKING, "GARBAGE BLOCKING"},
            {LANGUAGE, "LANGUAGE"},
            {SETTINGS, "SETTINGS"},
            {YES, "YES"},
            {NO, "NO"},
        }
    },
    {
        Language::ES, 
        {
            {START, "INICIAR"},
            {OPTIONS, "OPCIONES"},
            {SCORES, "PUNTUACIONES"},
            {VOLUME, "VOLUMEN"},
            {SEARCHING, "BUSCANDO"},
            {CONTROLLERS, "CONTROLES"},
            {CONTROLLER, "CONTROL"},
            {CONNECTED, "CONECTADO"},
            {RESUME, "REANUDAR"},
            {EXIT, "SALIR"},
            {PAUSE, "PAUSA"},
            {ZEN, "ZEN"},
            {MARATHON, "MARATON"},
            {VERSUS, "VERSUS"},
            {COOP, "COOPERATIVO"},
            {DUAL_BOARD, "DOS TABLEROS"},
            {GAME_MODE, "MODOS"},
            {VOLUME_SFX, "VOLUMEN SFX"},
            {VOLUME_MUSIC, "VOLUMEN MUSICA"},
            {UPCOMING_PIECES,"PIEZAS PROXIMAS"},
            {GAME_SCALE, "ESCALA"},
            {BOARD_WIDTH, "ANCHO TABLERO"},
            {BOARD_HEIGHT, "ALTO TABLERO"},
            {VELOCITY, "VELOCIDAD"},
            {LOCK_DELAY, "LOCK DELAY"},
            {MAX_VELOCITY, "MAX VELOCIDAD"},
            {GARBAGE_MULTIPLIER, "MULTIPLICADOR BASURA"},
            {GARBAGE_BLOCKING, "BLOQUEO BASURA"},
            {LANGUAGE, "IDIOMA"},
            {SETTINGS, "CONFIGURACIONES"},
            {YES, "SI"},
            {NO, "NO"}
        }
    },
};

inline const std::string& translate(Language language, Texts key)
{
    return translations[language][key];
}



