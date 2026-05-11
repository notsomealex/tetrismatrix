#include "libs.h"
#include "sqlite3.h"
#include "game.h"

enum class Language;

namespace DatabaseManager
{
    struct Player
    {
        int id;
        std::string name;
    };


    struct Match
    {
        int id;
        GameModes game_mode;
        std::string date;
    };

    struct Match_Player
    {
        int id;
        int match_id;
        int player_id;
        int score;
        int lines_cleared;
        bool won;
    };

    struct GameSettings
    {
        int id;
        int volume_music;
        int volume_sfx;
        Language language;
    };

    struct MatchSettings
    {
        // DEFAULT
        int max_amount_upcoming_pieces;
        int game_scale;
        int board_width;
        int board_height;
        int velocity;
        int lock_delay;

        // MARATHON
        int max_velocity;

        // VERSUS
        int garbage_multiplier;
        int garbage_blocking;

    };

    using ScoreEntry = Match_Player; 


    extern sqlite3* database;
    bool init();
    bool close();

    int savePlayer(const std::string& nickname);
    std::vector<Player> getAllPlayers();
    std::optional<Player> getPlayer(int id);

    int saveMatch(GameModes game_mode);
    std::vector<Match> getAllMatches();
    std::optional<Match> getMatch(int id);

    int saveScoreEntry(int match_id, int player_id, int score, int lines_cleared, bool won);
    std::vector<ScoreEntry> getAllScoreEntries();
    std::optional<ScoreEntry> getScoreEntry(int id);

    int saveGameSettings(int volume_music, int volume_sfx, Language language);
    std::vector<DatabaseManager::GameSettings> getAllGameSettings();
    std::optional<DatabaseManager::GameSettings> getGameSettings(int id);
    bool saveOrUpdateGameSettings(int volume_music, int volume_sfx, Language language);


};