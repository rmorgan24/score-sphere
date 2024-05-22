from tortoise import Model, fields

from score_sphere import enums

from .helpers import TimestampMixin


class Game(TimestampMixin, Model):
    id = fields.IntField(pk=True)

    home_team_name = fields.CharField(128, null=True)
    home_team_score = fields.IntField(default=0)

    away_team_name = fields.CharField(128, null=True)
    away_team_score = fields.IntField(default=0)

    status = fields.CharEnumField(
        enums.GameStatus, default=enums.GameStatus.NOT_STARTED
    )
    period = fields.IntField(default=1)
    time_remaining = fields.IntField(default=0)


class GameCard(TimestampMixin, Model):
    id = fields.IntField(pk=True)

    player_number = fields.IntField()
    card_color = fields.CharEnumField(enums.GameCardColor, max_length=16)
    period = fields.IntField()
    time_remaining = fields.IntField()

    game: fields.ForeignKeyRelation[Game] = fields.ForeignKeyField(
        "models.Game", related_name="cards"
    )

    class Meta:
        unique_together = (
            "game_id",
            "player_number",
            "period",
            "time_remaining",
        )
