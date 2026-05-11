#include "databasemanager.h"

using namespace DatabaseManager;
sqlite3* DatabaseManager::database = nullptr;
using ScoreEntry = Match_Player;

bool DatabaseManager::init()
{

    int connection_result = sqlite3_open("tetris.db", &database);
    if(connection_result != SQLITE_OK)
    {
        while(1) printf("error connecting to the database!\n\n");
        //printf("error connecting to the database!\n\n");
        return false;
    }

    const char* queries[] = {
        "CREATE TABLE IF NOT EXISTS player ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name VARCHAR(10) NOT NULL"
            ");",

        "CREATE TABLE IF NOT EXISTS game_mode ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name VARCHAR(20) NOT NULL"
            ");",

        "INSERT INTO game_mode (id, name) VALUES "
            "(0,'ZEN'),(1,'MARATHON'),(2,'VERSUS'),(3,'COOP'),(4,'DUAL BOARD') "
            "ON CONFLICT (id) DO NOTHING;",

        "CREATE TABLE IF NOT EXISTS match ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "game_mode_id INTEGER NOT NULL,"
            "date DATE"
            ");",

        "CREATE TABLE IF NOT EXISTS match_player ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "match_id INTEGER NOT NULL,"
            "player_id INTEGER NOT NULL,"
            "score INTEGER NOT NULL,"
            "lines_cleared INTEGER NOT NULL,"
            "won INTEGER NOT NULL,"
            "FOREIGN KEY (match_id) REFERENCES match(id),"
            "FOREIGN KEY (player_id) REFERENCES player(id)"
            ");",

        "CREATE TABLE IF NOT EXISTS language ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name VARCHAR(20)"
            ");",

        "INSERT INTO language (id, name) VALUES "
            "(0,'ENGLISH'),(1,'ESPANOL') "
            "ON CONFLICT (id) DO NOTHING;",

        "CREATE TABLE IF NOT EXISTS game_settings ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "volume_music INTEGER NOT NULL,"
            "volume_sfx INTEGER NOT NULL,"
            "language_id INTEGER NOT NULL"
            ");",

        "CREATE TABLE IF NOT EXISTS match_settings("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "max_amount_upcoming_pieces INTEGER NOT NULL,"
            "game_scale INTEGER NOT NULL,"
            "board_width INTEGER NOT NULL,"
            "board_height INTEGER NOT NULL,"
            "velocity INTEGER NOT NULL,"
            "lock_delay INTEGER NOT NULL,"
            "max_velocity INTEGER NOT NULL,"
            "garbage_multiplier INTEGER NOT NULL,"
            "garbage_blocking INTEGER NOT NULL"
            ");"
    };

    for(const char* query : queries) 
    {
        char* error = nullptr;
        int rc = sqlite3_exec(database, query, nullptr, nullptr, &error);
        if(rc != SQLITE_OK) 
        {
            printf("SQL error: %s\nquery: %s\n", error, query);
            sqlite3_free(error);
            return false;
        }
    }
    return true;
}

bool DatabaseManager::close()
{
    if(database != nullptr)
    {
        sqlite3_close(database);
        return true;
    }
    else
        return false;
}


// OKAY everything below is a TON of boilerplate but i guess there is not much i can do abt it sorry:/
int DatabaseManager::savePlayer(const std::string& nickname)
{
    sqlite3_stmt* stmt;
    const char* query = "INSERT INTO player (name) VALUES (?) RETURNING id;";
    
    int connection_return = sqlite3_prepare_v2(database, query, -1, &stmt, nullptr);
    if(connection_return != SQLITE_OK) {
        printf("prepare error: %s\n", sqlite3_errmsg(database));
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, nickname.c_str(), -1, SQLITE_STATIC);
    
    int id = -1;
    if(sqlite3_step(stmt) == SQLITE_ROW)
        id = sqlite3_column_int(stmt, 0);
    
    sqlite3_finalize(stmt);
    return id;
}

std::vector<Player> DatabaseManager::getAllPlayers()
{
    std::vector<Player> players;

    sqlite3_stmt* stmt;
    const char* query = "SELECT id, name FROM player;";

    int connection_result = sqlite3_prepare_v2(database, query, -1, &stmt, nullptr);
    if(connection_result != SQLITE_OK) 
    {
        printf("prepare error: %s\n", sqlite3_errmsg(database));
        return players;
    }

    while(sqlite3_step(stmt) == SQLITE_ROW) 
    {
        Player p;
        p.id = sqlite3_column_int(stmt, 0);
        p.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        players.push_back(p);
    }

    sqlite3_finalize(stmt);
    return players;
}

std::optional<Player> DatabaseManager::getPlayer(int id)
{
    sqlite3_stmt* stmt;
    const char* query = "SELECT id, name FROM player WHERE id = ?;";

    int rc = sqlite3_prepare_v2(database, query, -1, &stmt, nullptr);
    if(rc != SQLITE_OK) 
    {
        printf("prepare error: %s\n", sqlite3_errmsg(database));
        return std::nullopt;
    }

    sqlite3_bind_int(stmt, 1, id);

    std::optional<Player> result = std::nullopt;
    if(sqlite3_step(stmt) == SQLITE_ROW) 
    {
        Player p;
        p.id = sqlite3_column_int(stmt, 0);
        p.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        result = p;
    }

    sqlite3_finalize(stmt);
    return result;
}

int DatabaseManager::saveMatch(GameModes game_mode)
{
    sqlite3_stmt* stmt;
    const char* query = "INSERT INTO match (game_mode_id, date) VALUES (?, ?) RETURNING id;";

    int rc = sqlite3_prepare_v2(database, query, -1, &stmt, nullptr);
    if(rc != SQLITE_OK) {
        printf("prepare error: %s\n", sqlite3_errmsg(database));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, (int)game_mode);
    sqlite3_bind_int(stmt, 2, to_ms_since_boot(get_absolute_time()));

    int id = -1;
    if(sqlite3_step(stmt) == SQLITE_ROW)
        id = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    return id;
}

std::vector<DatabaseManager::Match> DatabaseManager::getAllMatches()
{
    std::vector<Match> matches;
    sqlite3_stmt* stmt;
    const char* query = "SELECT id, game_mode_id, date FROM match;";

    int rc = sqlite3_prepare_v2(database, query, -1, &stmt, nullptr);
    if(rc != SQLITE_OK) 
    {
        printf("prepare error: %s\n", sqlite3_errmsg(database));
        return matches;
    }

    while(sqlite3_step(stmt) == SQLITE_ROW) 
    {
        Match m;
        m.id = sqlite3_column_int(stmt, 0);
        m.game_mode = (GameModes)sqlite3_column_int(stmt, 1);
        m.date = sqlite3_column_int(stmt, 2);
        matches.push_back(m);
    }

    sqlite3_finalize(stmt);
    return matches;
}

std::optional<DatabaseManager::Match> DatabaseManager::getMatch(int id)
{
    sqlite3_stmt* stmt;
    const char* query = "SELECT id, game_mode_id, date FROM match WHERE id = ?;";

    int rc = sqlite3_prepare_v2(database, query, -1, &stmt, nullptr);
    if(rc != SQLITE_OK) 
    {
        printf("prepare error: %s\n", sqlite3_errmsg(database));
        return std::nullopt;
    }

    sqlite3_bind_int(stmt, 1, id);

    std::optional<Match> result;
    if(sqlite3_step(stmt) == SQLITE_ROW) 
    {
        Match m;
        m.id = sqlite3_column_int(stmt, 0);
        m.game_mode = (GameModes)sqlite3_column_int(stmt, 1);
        m.date = sqlite3_column_int(stmt, 2);
        result = m;
    }

    sqlite3_finalize(stmt);
    return result;
}

int DatabaseManager::saveScoreEntry(int match_id, int player_id, int score, int lines_cleared, bool won)
{
    sqlite3_stmt* stmt;
    const char* query = "INSERT INTO match_player (match_id, player_id, score, lines_cleared, won) VALUES (?, ?, ?, ?, ?) RETURNING id;";

    int rc = sqlite3_prepare_v2(database, query, -1, &stmt, nullptr);
    if(rc != SQLITE_OK) 
    {
        printf("prepare error: %s\n", sqlite3_errmsg(database));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, match_id);
    sqlite3_bind_int(stmt, 2, player_id);
    sqlite3_bind_int(stmt, 3, score);
    sqlite3_bind_int(stmt, 4, lines_cleared);
    sqlite3_bind_int(stmt, 5, won ? 1 : 0);

    int id = -1;
    if(sqlite3_step(stmt) == SQLITE_ROW)
        id = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    return id;
}

std::vector<ScoreEntry> DatabaseManager::getAllScoreEntries()
{
    std::vector<ScoreEntry> entries;
    sqlite3_stmt* stmt;
    const char* query = "SELECT id, match_id, player_id, score, lines_cleared, won FROM match_player;";

    int rc = sqlite3_prepare_v2(database, query, -1, &stmt, nullptr);
    if(rc != SQLITE_OK) {
        printf("prepare error: %s\n", sqlite3_errmsg(database));
        return entries;
    }

    while(sqlite3_step(stmt) == SQLITE_ROW) {
        ScoreEntry e;
        e.id = sqlite3_column_int(stmt, 0);
        e.match_id = sqlite3_column_int(stmt, 1);
        e.player_id = sqlite3_column_int(stmt, 2);
        e.score = sqlite3_column_int(stmt, 3);
        e.lines_cleared = sqlite3_column_int(stmt, 4);
        e.won = sqlite3_column_int(stmt, 5) == 1;
        entries.push_back(e);
    }

    sqlite3_finalize(stmt);
    return entries;
}

std::optional<ScoreEntry> DatabaseManager::getScoreEntry(int id)
{
    sqlite3_stmt* stmt;
    const char* query = "SELECT id, match_id, player_id, score, lines_cleared, won FROM match_player WHERE id = ?;";

    int rc = sqlite3_prepare_v2(database, query, -1, &stmt, nullptr);
    if(rc != SQLITE_OK) {
        printf("prepare error: %s\n", sqlite3_errmsg(database));
        return std::nullopt;
    }

    sqlite3_bind_int(stmt, 1, id);

    std::optional<ScoreEntry> result;
    if(sqlite3_step(stmt) == SQLITE_ROW) {
        ScoreEntry e;
        e.id = sqlite3_column_int(stmt, 0);
        e.match_id = sqlite3_column_int(stmt, 1);
        e.player_id = sqlite3_column_int(stmt, 2);
        e.score = sqlite3_column_int(stmt, 3);
        e.lines_cleared = sqlite3_column_int(stmt, 4);
        e.won = sqlite3_column_int(stmt, 5) == 1;
        result = e;
    }

    sqlite3_finalize(stmt);
    return result;
}


int DatabaseManager::saveGameSettings(int volume_music, int volume_sfx, Language language)
{
    sqlite3_stmt* stmt;
    const char* query = "INSERT INTO game_settings (volume_music, volume_sfx, language_id) VALUES (?, ?, ?) RETURNING id;";

    int rc = sqlite3_prepare_v2(database, query, -1, &stmt, nullptr);
    if(rc != SQLITE_OK) {
        printf("prepare error: %s\n", sqlite3_errmsg(database));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, volume_music);
    sqlite3_bind_int(stmt, 2, volume_sfx);
    sqlite3_bind_int(stmt, 3, (int)language);

    int id = -1;
    if(sqlite3_step(stmt) == SQLITE_ROW)
        id = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    return id;
}

std::vector<DatabaseManager::GameSettings> DatabaseManager::getAllGameSettings()
{
    std::vector<GameSettings> settings;
    sqlite3_stmt* stmt;
    const char* query = "SELECT id, volume_music, volume_sfx, language_id FROM game_settings;";

    int rc = sqlite3_prepare_v2(database, query, -1, &stmt, nullptr);
    if(rc != SQLITE_OK) 
    {
        printf("prepare error: %s\n", sqlite3_errmsg(database));
        return settings;
    }

    while(sqlite3_step(stmt) == SQLITE_ROW) 
    {
        GameSettings s;
        s.id = sqlite3_column_int(stmt, 0);
        s.volume_music = sqlite3_column_int(stmt, 1);
        s.volume_sfx = sqlite3_column_int(stmt, 2);
        s.language = (Language)sqlite3_column_int(stmt, 3);
        settings.push_back(s);
    }

    sqlite3_finalize(stmt);
    return settings;
}

std::optional<DatabaseManager::GameSettings> DatabaseManager::getGameSettings(int id)
{
    sqlite3_stmt* stmt;
    const char* query = "SELECT id, volume_music, volume_sfx, language_id FROM game_settings WHERE id = ?;";

    int rc = sqlite3_prepare_v2(database, query, -1, &stmt, nullptr);
    if(rc != SQLITE_OK) 
    {
        printf("prepare error: %s\n", sqlite3_errmsg(database));
        return std::nullopt;
    }

    sqlite3_bind_int(stmt, 1, id);

    std::optional<GameSettings> result;
    if(sqlite3_step(stmt) == SQLITE_ROW) 
    {
        GameSettings s;
        s.id = sqlite3_column_int(stmt, 0);
        s.volume_music = sqlite3_column_int(stmt, 1);
        s.volume_sfx = sqlite3_column_int(stmt, 2);
        s.language = (Language)sqlite3_column_int(stmt, 3);
        result = s;
    }

    sqlite3_finalize(stmt);
    return result;
}

// for now, there is likely just gonna be one settings per game, not per player or whatever, so this is useful
bool DatabaseManager::saveOrUpdateGameSettings(int volume_music, int volume_sfx, Language language)
{
    sqlite3_stmt* stmt;
    const char* query = "INSERT OR REPLACE INTO game_settings (id, volume_music, volume_sfx, language_id) VALUES (1, ?, ?, ?);";

    int rc = sqlite3_prepare_v2(database, query, -1, &stmt, nullptr);
    if(rc != SQLITE_OK) 
    {
        printf("prepare error: %s\n", sqlite3_errmsg(database));
        return false;
    }

    sqlite3_bind_int(stmt, 1, volume_music);
    sqlite3_bind_int(stmt, 2, volume_sfx);
    sqlite3_bind_int(stmt, 3, (int)language);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}