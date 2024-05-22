from quart import Blueprint
from quart_auth import current_user
from quart_schema import validate_querystring, validate_request, validate_response
from tortoise.transactions import atomic

from score_sphere import actions, schemas

blueprint = Blueprint("game", __name__)


@blueprint.post("")
@validate_request(schemas.GameCreate)
@validate_response(schemas.Game, 201)
@atomic()
async def create(data: schemas.GameCreate) -> schemas.Game:
    return await actions.game.create(await current_user.get_user(), data), 201


@blueprint.get("/<int:id>")
@validate_querystring(schemas.GameGetOptions)
@validate_response(schemas.Game, 200)
@atomic()
async def read(id: int, query_args: schemas.GameGetOptions) -> schemas.Game:
    return (
        await actions.game.get(
            await current_user.get_user(), id=id, options=query_args
        ),
        200,
    )


@blueprint.get("")
@validate_querystring(schemas.GameQueryString)
@validate_response(schemas.GameResultSet, 200)
@atomic()
async def read_many(query_args: schemas.GameQueryString) -> schemas.GameResultSet:
    return await actions.game.query(
        await current_user.get_user(), query_args.to_query()
    )


@blueprint.patch("/<int:id>")
@validate_request(schemas.GamePatch)
@validate_response(schemas.Game, 200)
@atomic()
async def update(id: int, data: schemas.GamePatch) -> schemas.Game:
    return await actions.game.update(await current_user.get_user(), id, data)


@blueprint.delete("/<int:id>")
@validate_response(schemas.DeleteConfirmed, 200)
@atomic()
async def delete(id: int) -> schemas.DeleteConfirmed:
    await actions.game.delete(await current_user.get_user(), id)

    return schemas.DeleteConfirmed(id=id)


@blueprint.put("/<int:id>/card")
@validate_request(schemas.GameCardCreate)
@validate_response(schemas.GameCard, 200)
@atomic()
async def card_create(id: int, data: schemas.GameCardCreate) -> schemas.GameCard:
    return await actions.game.card_create(await current_user.get_user(), id, data)
