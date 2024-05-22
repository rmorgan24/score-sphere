from tortoise import BaseDBAsyncClient


async def upgrade(db: BaseDBAsyncClient) -> str:
    return """
        ALTER TABLE "game" ADD "sport" VARCHAR(12) NULL;
        UPDATE "game" SET sport='other';
        ALTER TABLE "game" ALTER COLUMN sport SET NOT NULL;"""


async def downgrade(db: BaseDBAsyncClient) -> str:
    return """
        ALTER TABLE "game" DROP COLUMN "sport";"""
