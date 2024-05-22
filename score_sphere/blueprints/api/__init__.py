from quart import Blueprint

from .auth import blueprint as auth_blueprint
from .game import blueprint as game_blueprint
from .message import blueprint as message_blueprint
from .token import blueprint as token_blueprint
from .user import blueprint as user_blueprint

blueprint = Blueprint("api", __name__)

blueprint.register_blueprint(auth_blueprint, url_prefix="/auth")
blueprint.register_blueprint(game_blueprint, url_prefix="/game")
blueprint.register_blueprint(token_blueprint, url_prefix="/token")
blueprint.register_blueprint(user_blueprint, url_prefix="/user")
blueprint.register_blueprint(message_blueprint, url_prefix="/message")
