from tortoise import BaseDBAsyncClient


async def upgrade(db: BaseDBAsyncClient) -> str:
    return """
        CREATE TABLE IF NOT EXISTS "game" (
    "created_at" TIMESTAMPTZ NOT NULL  DEFAULT CURRENT_TIMESTAMP,
    "modified_at" TIMESTAMPTZ NOT NULL  DEFAULT CURRENT_TIMESTAMP,
    "id" SERIAL NOT NULL PRIMARY KEY,
    "home_team_name" VARCHAR(128),
    "home_team_score" INT NOT NULL  DEFAULT 0,
    "away_team_name" VARCHAR(128),
    "away_team_score" INT NOT NULL  DEFAULT 0,
    "period" INT NOT NULL  DEFAULT 1,
    "time_remaining" INT NOT NULL  DEFAULT 0
);
        CREATE TABLE IF NOT EXISTS "gamecard" (
    "created_at" TIMESTAMPTZ NOT NULL  DEFAULT CURRENT_TIMESTAMP,
    "modified_at" TIMESTAMPTZ NOT NULL  DEFAULT CURRENT_TIMESTAMP,
    "id" SERIAL NOT NULL PRIMARY KEY,
    "player_number" INT NOT NULL,
    "card_color" VARCHAR(16) NOT NULL,
    "period" INT NOT NULL,
    "time_remaining" INT NOT NULL,
    "game_id" INT NOT NULL REFERENCES "game" ("id") ON DELETE CASCADE,
    CONSTRAINT "uid_gamecard_game_id_d9f1cd" UNIQUE ("game_id", "player_number", "period", "time_remaining")
);
COMMENT ON COLUMN "gamecard"."card_color" IS 'RED: red\nYELLOW: yellow';"""


async def downgrade(db: BaseDBAsyncClient) -> str:
    return """
        DROP TABLE IF EXISTS "game";
        DROP TABLE IF EXISTS "gamecard";"""
