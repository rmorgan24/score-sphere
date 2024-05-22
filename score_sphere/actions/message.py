from typing import Union

from tortoise.exceptions import DoesNotExist
from tortoise.expressions import F, Q

from score_sphere import enums, models, schemas
from score_sphere.lib.error import ActionError, ForbiddenActionError

from .helpers import conditional_set, handle_orm_errors


def has_permission(
    _user: schemas.User,
    _obj: Union[schemas.Message, None],
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
    user: schemas.User, id: int = None, options: schemas.MessageGetOptions = None
) -> schemas.Message:
    game = None
    if id:
        game = await models.Message.get(id=id)
    else:
        raise ActionError("missing lookup key", type="not_found")

    if not has_permission(
        user, schemas.Message.model_validate(game), enums.Permission.READ
    ):
        raise ForbiddenActionError()

    if options:
        if options.resolves:
            await game.fetch_related(*options.resolves)

    return schemas.Message.model_validate(game)


@handle_orm_errors
async def query(
    _user: schemas.User, q: schemas.MessageQuery
) -> schemas.MessageResultSet:
    qs = models.Message.all()

    queryset, pagination = await q.apply(qs)

    return schemas.MessageResultSet(
        pagination=pagination,
        games=[schemas.Message.model_validate(game) for game in await queryset],
    )


@handle_orm_errors
async def create(user: schemas.User, data: schemas.MessageCreate) -> schemas.Message:
    if not has_permission(user, None, enums.Permission.CREATE):
        raise ForbiddenActionError()

    game = await models.Message.create(text=data.text)

    return schemas.Message.model_validate(game)


@handle_orm_errors
async def delete(user: schemas.User, id: int) -> None:
    game = await models.Message.get(id=id)

    if not has_permission(
        user, schemas.Message.model_validate(game), enums.Permission.DELETE
    ):
        raise ForbiddenActionError()

    await game.delete()


@handle_orm_errors
async def update(
    user: schemas.User, id: int, data: schemas.MessagePatch
) -> schemas.Message:
    game = await models.Message.get(id=id)

    if not has_permission(
        user, schemas.Message.model_validate(game), enums.Permission.UPDATE
    ):
        raise ForbiddenActionError()

    conditional_set(game, "text", data.text)

    await game.save()

    return schemas.Message.model_validate(game)
