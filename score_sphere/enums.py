from enum import Enum


class EnumStr(str, Enum):
    def __str__(self):
        return self.value


class UserRole(EnumStr):
    ADMIN = "admin"
    USER = "user"


class UserStatus(EnumStr):
    PENDING = "pending"
    ACTIVE = "active"


class Permission(EnumStr):
    CREATE = "create"
    READ = "read"
    UPDATE = "update"
    DELETE = "delete"


class TokenType(EnumStr):
    WEB = "web"
    API = "api"


class GameStatus(EnumStr):
    NOT_STARTED = "not-started"
    IN_PROGRESS = "in-progress"
    ENDED = "ended"


class GameTeam(EnumStr):
    HOME = "home"
    AWAY = "away"


class GameCardColor(EnumStr):
    RED = "red"
    YELLOW = "yellow"


class GameSport(EnumStr):
    BASKETBALL = "basketball"
    FIELD_HOCKEY = "field-hockey"
    LACROSSE = "lacrosse"
    SOCCER = "soccer"
    OTHER = "other"
