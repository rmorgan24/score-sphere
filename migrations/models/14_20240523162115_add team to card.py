from tortoise import BaseDBAsyncClient


async def upgrade(db: BaseDBAsyncClient) -> str:
    return """
        ALTER TABLE "gamecard" ADD "team" VARCHAR(4) NULL;
        UPDATE "gamecard" SET team='away';
        ALTER TABLE "gamecard" ALTER COLUMN team SET NOT NULL;"""


async def downgrade(db: BaseDBAsyncClient) -> str:
    return """
        ALTER TABLE "gamecard" DROP COLUMN "team";"""
