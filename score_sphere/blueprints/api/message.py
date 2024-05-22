from quart import Blueprint
from quart_auth import current_user
from quart_schema import validate_querystring, validate_request, validate_response
from tortoise.transactions import atomic

from score_sphere import actions, schemas

blueprint = Blueprint("message", __name__)


@blueprint.post("")
@validate_request(schemas.MessageCreate)
@validate_response(schemas.Message, 201)
@atomic()
async def create(data: schemas.MessageCreate) -> schemas.Message:
    return await actions.message.create(await current_user.get_user(), data), 201


@blueprint.get("/<int:id>")
@validate_querystring(schemas.MessageGetOptions)
@validate_response(schemas.Message, 200)
@atomic()
async def read(id: int, query_args: schemas.MessageGetOptions) -> schemas.Message:
    return (
        await actions.message.get(
            await current_user.get_user(), id=id, options=query_args
        ),
        200,
    )


@blueprint.get("")
@validate_querystring(schemas.MessageQueryString)
@validate_response(schemas.MessageResultSet, 200)
@atomic()
async def read_many(query_args: schemas.MessageQueryString) -> schemas.MessageResultSet:
    return await actions.message.query(
        await current_user.get_user(), query_args.to_query()
    )


@blueprint.patch("/<int:id>")
@validate_request(schemas.MessagePatch)
@validate_response(schemas.Message, 200)
@atomic()
async def update(id: int, data: schemas.MessagePatch) -> schemas.Message:
    return await actions.message.update(await current_user.get_user(), id, data)


@blueprint.delete("/<int:id>")
@validate_response(schemas.DeleteConfirmed, 200)
@atomic()
async def delete(id: int) -> schemas.DeleteConfirmed:
    await actions.message.delete(await current_user.get_user(), id)

    return schemas.DeleteConfirmed(id=id)
