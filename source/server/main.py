from fastapi import FastAPI
from pydantic import BaseModel
import mysql.connector
from typing import Optional
import os
from dotenv import load_dotenv

load_dotenv()
app = FastAPI()

def get_db():
    return mysql.connector.connect(
        host=os.getenv("DB_HOST"),
        port=int(os.getenv("DB_PORT", 3306)),
        user=os.getenv("DB_USER"),
        password=os.getenv("DB_PASSWORD"),
        database=os.getenv("DB_NAME")
    )

# Models
class Player(BaseModel):
    name: str

class Match(BaseModel):
    game_mode_id: int

class ScoreEntry(BaseModel):
    match_id: int
    player_id: int
    score: int
    lines_cleared: int
    won: bool
    singles: int
    doubles: int
    triples: int
    tetris: int

# Endpoints
@app.post("/players")
def insert_player(player: Player):
    db = get_db()
    cursor = db.cursor()
    cursor.execute("INSERT INTO player(name) VALUES(%s)", (player.name,))
    db.commit()
    return {"id": cursor.lastrowid}

@app.post("/players/find_or_create")
def find_or_create_player(player: Player):
    db = get_db()
    cursor = db.cursor(dictionary=True)
    cursor.execute("SELECT * FROM player WHERE name = %s", (player.name,))
    existing = cursor.fetchone()
    if existing:
        return existing
    cursor.execute("INSERT INTO player(name) VALUES(%s)", (player.name,))
    db.commit()
    return {"id": cursor.lastrowid, "name": player.name}

@app.get("/players/all_stats")
def get_all_players_stats():
    db = get_db()
    cursor = db.cursor(dictionary=True)
    cursor.execute("""
        SELECT 
            p.id,
            p.name,
            COUNT(mp.match_id)        as matches_played,
            COALESCE(SUM(mp.score), 0)         as total_score,
            COALESCE(SUM(mp.lines_cleared), 0) as total_lines_cleared,
            COALESCE(SUM(mp.singles), 0)       as total_singles,
            COALESCE(SUM(mp.doubles), 0)       as total_doubles,
            COALESCE(SUM(mp.triples), 0)       as total_triples,
            COALESCE(SUM(mp.tetris), 0)        as total_tetris,
            COALESCE(SUM(mp.won), 0)           as matches_won
        FROM player p
        LEFT JOIN match_player mp ON p.id = mp.player_id
        GROUP BY p.id, p.name
        ORDER BY total_score DESC
    """)
    return cursor.fetchall()

@app.get("/players/stats")
def get_player_stats(id: int):
    db = get_db()
    cursor = db.cursor(dictionary=True)
    cursor.execute("""
        SELECT 
            p.id,
            p.name,
            COUNT(mp.match_id)        as matches_played,
            COALESCE(SUM(mp.score), 0)         as total_score,
            COALESCE(SUM(mp.lines_cleared), 0) as total_lines_cleared,
            COALESCE(SUM(mp.singles), 0)       as total_singles,
            COALESCE(SUM(mp.doubles), 0)       as total_doubles,
allCOALESCE(SUM(mp.triples), 0)       as total_triples,
            COALESCE(SUM(mp.tetris), 0)        as total_tetris,
            COALESCE(SUM(mp.won), 0)           as matches_won
        FROM player p
        WHERE p.id = %s
        LEFT JOIN match_player mp ON p.id = mp.player_id
        GROUP BY p.id, p.name
        ORDER BY total_score DESC LIMIT %s
    """, (id,))
    return cursor.fetchall()


@app.get("/scores")
def get_all_scores():
    db = get_db()
    cursor = db.cursor(dictionary=True)
    cursor.execute("SELECT * FROM match_player ORDER BY score DESC")
    return cursor.fetchall()

@app.get("/scores/detailed/{amount}")
def get_top_detailed_scores(amount: int):
    db = get_db()
    cursor = db.cursor(dictionary=True)
    cursor.execute("""
        SELECT mp.*, p.name as player_name 
        FROM match_player mp
        JOIN player p ON mp.player_id = p.id
        ORDER BY mp.score DESC LIMIT %s 
    """, (amount,))
    return cursor.fetchall()
@app.post("/match")
def insert_match(match: Match):
    db = get_db()
    cursor = db.cursor()
    cursor.execute("INSERT INTO `match`(game_mode_id, date) VALUES(%s, CURDATE())", (match.game_mode_id,))
    db.commit()
    return {"id": cursor.lastrowid}

@app.post("/score_entries")
def insert_score_entry(entry: ScoreEntry):
    db = get_db()
    cursor = db.cursor()
    cursor.execute(
        "INSERT INTO match_player(match_id, player_id, score, lines_cleared, won, singles, doubles, triples, tetris) VALUES(%s,%s,%s,%s,%s,%s,%s,%s,%s)",
        (entry.match_id, entry.player_id, entry.score, entry.lines_cleared, entry.won, entry.singles, entry.doubles, entry.triples, entry.tetris)
    )
    db.commit()
    return {"id": cursor.lastrowid}

@app.get("/players/{id}")
def get_player(id: int):
    db = get_db()
    cursor = db.cursor(dictionary=True)
    cursor.execute("SELECT * FROM player WHERE id = %s", (id,))
    result = cursor.fetchone()
    return result or None

@app.get("/matches/{id}")
def get_match(id: int):
    db = get_db()
    cursor = db.cursor(dictionary=True)
    cursor.execute("SELECT * FROM `match` WHERE id = %s", (id,))
    result = cursor.fetchone()
    return result or None



@app.get("/score_entries/{id}")
def get_score_entry(id: int):
    db = get_db()
    cursor = db.cursor(dictionary=True)
    cursor.execute("SELECT * FROM match_player WHERE id = %s", (id,))
    result = cursor.fetchone()
    return result or None

@app.get("/game_settings/{id}")
def get_game_settings(id: int):
    db = get_db()
    cursor = db.cursor(dictionary=True)
    cursor.execute("SELECT * FROM game_settings WHERE id = %s", (id,))
    result = cursor.fetchone()
    return result or None

@app.get("/players")
def get_all_players():
    db = get_db()
    cursor = db.cursor(dictionary=True)
    cursor.execute("SELECT * FROM player")
    return cursor.fetchall()

@app.get("/score_entries")
def get_all_score_entries():
    db = get_db()
    cursor = db.cursor(dictionary=True)
    cursor.execute("SELECT * FROM match_player ORDER BY score DESC")
    return cursor.fetchall()

@app.get("/amount_score_entries/{amount}")
def get_amount_score_entries(amount: int):
    db = get_db()
    cursor = db.cursor(dictionary=True)
    cursor.execute("SELECT * FROM match_player ORDER BY score DESC LIMIT %s", (amount,))
    return cursor.fetchall()

