import json
from typing import Union

from tortoise.exceptions import DoesNotExist

from score_sphere import enums, models, schemas
from score_sphere.lib.error import ActionError, ForbiddenActionError
from score_sphere.lib.message_manager import MessageManager

from .helpers import conditional_set, handle_orm_errors


def has_permission(
    _user: schemas.User,
    _obj: Union[schemas.Game, None],
    permission: enums.Permission,
) -> bool:
    if permission == enums.Permission.CREATE:
        return True

    if permission == enums.Permission.READ:
        return True

    if permission == enums.Permission.UPDATE:
        return True

    if permission == enums.Permission.DELETE:
        return True

    return False


@handle_orm_errors
async def get(
    user: schemas.User, id: int = None, options: schemas.GameGetOptions = None
) -> schemas.Game:
    game = None
    if id:
        game = await models.Game.get(id=id)
    else:
        raise ActionError("missing lookup key", type="not_found")

    if not has_permission(
        user, schemas.Game.model_validate(game), enums.Permission.READ
    ):
        raise ForbiddenActionError()

    if options:
        if options.resolves:
            await game.fetch_related(*options.resolves)

    return schemas.Game.model_validate(game)


@handle_orm_errors
async def query(_user: schemas.User, q: schemas.GameQuery) -> schemas.GameResultSet:
    qs = models.Game.all()

    queryset, pagination = await q.apply(qs)

    return schemas.GameResultSet(
        pagination=pagination,
        games=[schemas.Game.model_validate(game) for game in await queryset],
    )


@handle_orm_errors
async def create(user: schemas.User, data: schemas.GameCreate) -> schemas.Game:
    if not has_permission(user, None, enums.Permission.CREATE):
        raise ForbiddenActionError()

    game = await models.Game.create(
        sport=data.sport,
        home_team_name=data.home_team_name,
        away_team_name=data.away_team_name,
        period=data.period,
        time_remaining=data.time_remaining,
        status=data.status,
    )

    return schemas.Game.model_validate(game)


@handle_orm_errors
async def delete(user: schemas.User, id: int) -> None:
    game = await models.Game.get(id=id)

    if not has_permission(
        user, schemas.Game.model_validate(game), enums.Permission.DELETE
    ):
        raise ForbiddenActionError()

    await game.delete()


@handle_orm_errors
async def update(user: schemas.User, id: int, data: schemas.GamePatch) -> schemas.Game:
    game = await models.Game.get(id=id)

    if not has_permission(
        user, schemas.Game.model_validate(game), enums.Permission.UPDATE
    ):
        raise ForbiddenActionError()

    conditional_set(game, "sport", data.sport)

    conditional_set(game, "home_team_name", data.home_team_name)
    conditional_set(game, "home_team_score", data.home_team_score)

    conditional_set(game, "away_team_name", data.away_team_name)
    conditional_set(game, "away_team_score", data.away_team_score)

    conditional_set(game, "status", data.status)
    conditional_set(game, "period", data.period)
    conditional_set(game, "time_remaining", data.time_remaining)

    await game.save()
    await game.fetch_related("cards")

    schema_game = schemas.Game.model_validate(game)

    mm = MessageManager(user, f"game-{id}")
    await mm.send_message(
        "update",
        f"Game {id} updated",
        data=json.loads(schemas.Game.model_dump_json(schema_game)),
    )

    return schema_game


@handle_orm_errors
async def card_get(user: schemas.User, id: int) -> schemas.GameCard:
    game_card = await models.GameCard.get(game_id=id, user_id=user.id)
    return schemas.GameCard.model_validate(game_card)


@handle_orm_errors
async def card_create(
    user: schemas.User, id: int, data: schemas.GameCardCreate
) -> schemas.GameCard:
    game = await models.Game.get(id=id)

    if not has_permission(
        user, schemas.Game.model_validate(game), enums.Permission.READ
    ):
        raise ForbiddenActionError()

    try:
        game_card = await models.GameCard.get(
            game_id=id,
            team=data.team,
            player_number=data.player_number,
            period=data.period,
            time_remaining=data.time_remaining,
        )
    except DoesNotExist:
        game_card = await models.GameCard.create(
            game_id=id,
            card_color=data.card_color,
            team=data.team,
            player_number=data.player_number,
            period=data.period,
            time_remaining=data.time_remaining,
        )

    game_card.card_color = data.card_color
    await game_card.save()

    await game.fetch_related("cards")

    schema_game = schemas.Game.model_validate(game)

    mm = MessageManager(user, f"game-{id}")
    await mm.send_message(
        "update",
        f"Game {id} updated",
        data=json.loads(schemas.Game.model_dump_json(schema_game)),
    )

    return schemas.GameCard.model_validate(game_card)
