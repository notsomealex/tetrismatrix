#include "databasemanager.h"
#include "networkmanager.h"
#include "types.h"


// helpers

using namespace DatabaseManager;


int DatabaseManager::parseId(const std::string& response) 
{
    std::string body = NetworkManager::extractBody(response);
    size_t pos = body.find("\"id\":");
    if(pos == std::string::npos) return -1;
    return std::stoi(  body.substr(pos + 5));
}

// players
int DatabaseManager::savePlayer(const std::string& name) 
{
    std::string body = "{\"name\":\"" + name + "\"}";
    std::string response = NetworkManager::httpPost(SERVER_URL, PORT, "/players/find_or_create", body.c_str());
    return parseId(response);
}
std::optional<Player> DatabaseManager::getPlayer(int id) 
{
    std::string response = NetworkManager::httpGet(SERVER_URL, PORT, ("/players/" + std::to_string(id)).c_str());
    std::string body = NetworkManager::extractBody(response);
    printf("getPlayer response: %s\n", response.c_str());
    if(body.empty()) 
    {
        return std::nullopt;
    }
    Player p;
    p.id = id;
    size_t name_pos = body.find("\"name\":\"");
    if(name_pos == std::string::npos) return std::nullopt;
    size_t name_start = name_pos + 8;
    size_t name_end = body.find("\"", name_start);
    p.name = body.substr(name_start, name_end - name_start);
    return p;
}

// matches
int DatabaseManager::saveMatch(GameModes game_mode) 
{
    std::string body = "{\"game_mode_id\":" + std::to_string((int)game_mode) + "}";
    std::string response = NetworkManager::httpPost(SERVER_URL, PORT, "/match", body.c_str());
    return parseId(response);
}

// Matches
std::vector<Match> DatabaseManager::getAllMatches()
{
    std::vector<Match> matches;
    std::string response = NetworkManager::httpGet(SERVER_URL, PORT, "/matches");
    std::string body = NetworkManager::extractBody(response);
    
    size_t pos = 0;
    while((pos = body.find("\"id\":", pos)) != std::string::npos) {
        Match m;
        m.id = std::stoi(body.substr(pos + 5));
        
        size_t mode_pos = body.find("\"game_mode_id\":", pos);
        if(mode_pos != std::string::npos)
            m.game_mode = (GameModes)std::stoi(body.substr(mode_pos + 15));
        
        size_t date_pos = body.find("\"date\":\"", pos);
        if(date_pos != std::string::npos) {
            size_t date_start = date_pos + 8;
            size_t date_end = body.find("\"", date_start);
            m.date = body.substr(date_start, date_end - date_start);
        }
        matches.push_back(m);
        pos++;
    }
    return matches;
}

std::optional<Match> DatabaseManager::getMatch(int id)
{
    std::string response = NetworkManager::httpGet(SERVER_URL, PORT, ("/matches/" + std::to_string(id)).c_str());
    std::string body = NetworkManager::extractBody(response);
    if(body.find("null") != std::string::npos || body.empty()) return std::nullopt;
    
    Match m;
    m.id = id;
    size_t mode_pos = body.find("\"game_mode_id\":");
    if(mode_pos != std::string::npos)
        m.game_mode = (GameModes)std::stoi(body.substr(mode_pos + 15));
    size_t date_pos = body.find("\"date\":\"");
    if(date_pos != std::string::npos) {
        size_t date_start = date_pos + 8;
        size_t date_end = body.find("\"", date_start);
        m.date = body.substr(date_start, date_end - date_start);
    }
    return m;
}

// score entries
int DatabaseManager::saveScoreEntry(int match_id, int player_id, int score, int lines_cleared, bool won, int singles, int doubles, int triples, int tetris) 
{
    std::string body = "{\"match_id\":" + std::to_string(match_id) +
                      ",\"player_id\":" + std::to_string(player_id) +
                      ",\"score\":" + std::to_string(score) +
                      ",\"lines_cleared\":" + std::to_string(lines_cleared) +
                      ",\"won\":" + (won ? "true" : "false") +
                      ",\"singles\":" + std::to_string(singles) +
                      ",\"doubles\":" + std::to_string(doubles) +
                      ",\"triples\":" + std::to_string(triples) +
                      ",\"tetris\":" + std::to_string(tetris) + "}";
    std::string response = NetworkManager::httpPost(SERVER_URL, PORT, "/score_entries", body.c_str());
    return parseId(response);
}
// Score entries
std::vector<ScoreEntry> DatabaseManager::getAllScoreEntries()
{
    std::vector<ScoreEntry> entries;
    std::string response = NetworkManager::httpGet(SERVER_URL, PORT, "/score_entries");
    std::string body = NetworkManager::extractBody(response);

    size_t pos = 0;
    while((pos = body.find("\"id\":", pos)) != std::string::npos) {
        ScoreEntry e;
        e.id = std::stoi(body.substr(pos + 5));

        auto findInt = [&](const std::string& key) -> int {
            size_t p = body.find(key, pos);
            if(p == std::string::npos) return 0;
            return std::stoi(body.substr(p + key.size()));
        };

        e.match_id = findInt("\"match_id\":");
        e.player_id = findInt("\"player_id\":");
        e.score = findInt("\"score\":");
        e.lines_cleared = findInt("\"lines_cleared\":");
        e.won = findInt("\"won\":") == 1;
        e.singles = findInt("\"singles\":");
        e.doubles = findInt("\"doubles\":");
        e.triples = findInt("\"triples\":");
        e.tetris = findInt("\"tetris\":");
        entries.push_back(e);
        pos++;
    }
    return entries;
}

std::vector<ScoreEntry> DatabaseManager::getHigherScoreEntries(int amount)
{
    std::vector<ScoreEntry> entries;
    std::string response = NetworkManager::httpGet(SERVER_URL, PORT, ("/amount_score_entries/" + std::to_string(amount)).c_str());
    std::string body = NetworkManager::extractBody(response);

    size_t pos = 0;
    while((pos = body.find("\"id\":", pos)) != std::string::npos) {
        ScoreEntry e;
        e.id = std::stoi(body.substr(pos + 5));

        auto findInt = [&](const std::string& key) -> int {
            size_t p = body.find(key, pos);
            if(p == std::string::npos) return 0;
            return std::stoi(body.substr(p + key.size()));
        };

        e.match_id = findInt("\"match_id\":");
        e.player_id = findInt("\"player_id\":");
        e.score = findInt("\"score\":");
        e.lines_cleared = findInt("\"lines_cleared\":");
        e.won = findInt("\"won\":") == 1;
        e.singles = findInt("\"singles\":");
        e.doubles = findInt("\"doubles\":");
        e.triples = findInt("\"triples\":");
        e.tetris = findInt("\"tetris\":");
        entries.push_back(e);
        pos++;
    }
    return entries;
}

std::optional<ScoreEntry> DatabaseManager::getScoreEntry(int id)
{
    std::string response = NetworkManager::httpGet(SERVER_URL, PORT, ("/score_entries/" + std::to_string(id)).c_str());
    std::string body = NetworkManager::extractBody(response);
    if(body.find("null") != std::string::npos || body.empty()) return std::nullopt;

    ScoreEntry e;
    e.id = id;
    auto findInt = [&](const std::string& key) -> int {
        size_t p = body.find(key);
        if(p == std::string::npos) return 0;
        return std::stoi(body.substr(p + key.size()));
    };
    e.match_id = findInt("\"match_id\":");
    e.player_id = findInt("\"player_id\":");
    e.score = findInt("\"score\":");
    e.lines_cleared = findInt("\"lines_cleared\":");
    e.won = findInt("\"won\":") == 1;
    e.singles = findInt("\"singles\":");
    e.doubles = findInt("\"doubles\":");
    e.triples = findInt("\"triples\":");
    e.tetris = findInt("\"tetris\":");
    return e;
}

// game settings
bool DatabaseManager::saveOrUpdateGameSettings(int volume_music, int volume_sfx, Language language) 
{
    std::string body = "{\"volume_music\":" + std::to_string(volume_music) +
                      ",\"volume_sfx\":" + std::to_string(volume_sfx) +
                      ",\"language_id\":" + std::to_string((int)language) + "}";
    std::string response = NetworkManager::httpPost(SERVER_URL, PORT, "/game_settings", body.c_str());
    return parseId(response) != -1;
}

