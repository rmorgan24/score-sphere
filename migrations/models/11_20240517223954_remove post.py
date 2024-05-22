from tortoise import BaseDBAsyncClient


async def upgrade(db: BaseDBAsyncClient) -> str:
    return """
        DROP TABLE IF EXISTS "postlike";
        DROP TABLE IF EXISTS "post";
        """


async def downgrade(db: BaseDBAsyncClient) -> str:
    return """
        ;"""
