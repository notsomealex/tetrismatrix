#include "libs.h"

enum class Language
{
    EN,
    ES
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
    GAME_MODE
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
            {GAME_MODE, "GAME MODE"}
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
            {GAME_MODE, "MODOS"}
        }
    },
};

inline const std::string& translate(Language language, Texts key)
{
    return translations[language][key];
}



