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
    CONNECTED
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
            {CONNECTED, "CONNECTED"}
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
            {CONNECTED, "CONECTADO"}
        }
    },
};

inline const std::string& translate(Language language, Texts key)
{
    return translations[language][key];
}



