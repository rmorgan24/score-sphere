import asyncio
import json
import uuid
from typing import Any

from quart import Blueprint, current_app, redirect, request, url_for, websocket
from quart.templating import render_template
from quart_auth import current_user, login_required
from quart_schema import validate_querystring

from score_sphere import actions, enums, schemas
from score_sphere.lib.auth import Forbidden
from score_sphere.lib.error import ActionError
from score_sphere.lib.message_manager import MessageManager

blueprint = Blueprint("game", __name__, template_folder="templates")


@blueprint.route("/")
@validate_querystring(schemas.GameQueryString)
async def index(query_args: schemas.GameQueryString):
    if not query_args.status__in:
        return redirect(
            url_for(
                ".index",
                status__in=f"{enums.GameStatus.NOT_STARTED},{enums.GameStatus.IN_PROGRESS}",
            )
        )
    user = await current_user.get_user()
    resultset = await actions.game.query(user, query_args.to_query(resolves=["cards"]))

    subtab = "all"
    if query_args.status__in and len(query_args.status__in) == 1:
        subtab = query_args.status__in[0]

    return await render_template(
        "game/index.html", resultset=resultset, tab="game", subtab=subtab
    )


@blueprint.route("/<int:id>")
async def view(id: int):
    user = await current_user.get_user()
    game = await actions.game.get(
        user, id=id, options=schemas.GameGetOptions(resolves=["cards"])
    )

    can_edit = actions.game.has_permission(user, game, enums.Permission.UPDATE)

    return await render_template("game/view.html", game=game, can_edit=can_edit)


@blueprint.route("/create/")
@login_required
async def create():
    user = await current_user.get_user()
    if not actions.game.has_permission(user, None, enums.Permission.CREATE):
        raise Forbidden()

    modal = 1 if "modal" in request.args else None

    game = schemas.Object()
    game.id = 0
    game.period = 1
    game.status = enums.GameStatus.NOT_STARTED

    return await render_template(
        "game/create.html",
        game=game,
        sport_options=[(x.value.title(), x.value) for x in enums.GameSport],
        status_options=[(x.value.title(), x.value) for x in enums.GameStatus],
        r=url_for(".index", status__in="{-status-}"),
        tab="game",
        base_template="modal_base.html" if modal else None,
    )


@blueprint.route("/<int:id>/edit/")
@login_required
async def update(id: int):
    user = await current_user.get_user()
    game = await actions.game.get(user, id=id)

    if not actions.game.has_permission(user, game, enums.Permission.UPDATE):
        raise Forbidden()

    modal = 1 if "modal" in request.args else None

    return await render_template(
        "game/update.html",
        game=game,
        sport_options=[(x.value.title(), x.value) for x in enums.GameSport],
        status_options=[(x.value.title(), x.value) for x in enums.GameStatus],
        r=url_for(".view", id=game.id),
        tab="game",
        base_template="modal_base.html" if modal else None,
    )


@blueprint.websocket("/<int:id>/ws")
async def ws(id: int) -> None:
    user = await current_user.get_user()
    game = await actions.game.get(user, id=id)

    if not actions.game.has_permission(user, game, enums.Permission.READ):
        raise Forbidden()

    async with MessageManager(
        user,
        f"game-{id}",
        session_id=websocket.args.get("SESSION_ID"),
    ) as mm:
        while True:
            data = await websocket.receive()
            await mm.send_message("message", data)
