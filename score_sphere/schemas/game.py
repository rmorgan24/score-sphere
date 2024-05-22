from datetime import datetime
from typing import List, Optional, Union

from pydantic import StringConstraints, ValidationInfo, computed_field, field_validator
from typing_extensions import Annotated

from score_sphere import enums

from .helpers import (
    NOTSET,
    BaseModel,
    PydanticValueError,
    parse_list,
    remove_reverse_relation,
)
from .pagination import PageInfo, Pagination
from .query import Query

CARD_COLOR_VALIDATOR = enums.GameCardColor
GAME_ID_VALIDATOR = int
GAME_STATUS_VALIDATOR = enums.GameStatus
PERIOD_VALIDATOR = int
PLAYER_NUMBER_VALIDATOR = int
TEAM_NAME_VALIDATOR = Annotated[str, StringConstraints(max_length=16)]
TEAM_SCORE_VALIDATOR = int
TIME_REMAINING_VALIDATOR = int


class GameCardCreate(BaseModel):
    game_id: GAME_ID_VALIDATOR
    player_number: PLAYER_NUMBER_VALIDATOR
    card_color: CARD_COLOR_VALIDATOR
    period: PERIOD_VALIDATOR
    time_remaining: TIME_REMAINING_VALIDATOR


class GameCard(BaseModel):
    id: int

    game_id: int

    player_number: int
    card_color: str
    period: int
    time_remaining: int


class GameCreate(BaseModel):
    home_team_name: Optional[TEAM_NAME_VALIDATOR] = None
    away_team_name: Optional[TEAM_NAME_VALIDATOR] = None
    time_remaining: TIME_REMAINING_VALIDATOR
    period: PERIOD_VALIDATOR
    status: GAME_STATUS_VALIDATOR = enums.GameStatus.NOT_STARTED

    @field_validator("time_remaining", mode="before")
    @classmethod
    def time_remaining_clock_converter(
        cls, value: Union[int, str], _info: ValidationInfo
    ):
        if isinstance(value, int):
            return value

        s_values = value.split(":")
        if len(s_values) == 2:
            try:
                s_values = [int(x) for x in s_values]
            except ValueError as e:
                raise PydanticValueError("Individual values must be integers") from e

            return s_values[0] * 60 + s_values[1]

        raise PydanticValueError("Invalid time format, please use 'XX:XX'")


class GamePatch(BaseModel):
    home_team_name: TEAM_NAME_VALIDATOR = NOTSET
    home_team_score: TEAM_SCORE_VALIDATOR = NOTSET

    away_team_name: TEAM_NAME_VALIDATOR = NOTSET
    away_team_score: TEAM_SCORE_VALIDATOR = NOTSET

    status: GAME_STATUS_VALIDATOR = NOTSET
    period: PERIOD_VALIDATOR = NOTSET
    time_remaining: TIME_REMAINING_VALIDATOR = NOTSET


class Game(BaseModel):
    id: int

    home_team_name: Optional[str]
    home_team_score: int

    away_team_name: Optional[str]
    away_team_score: int

    status: str
    period: int
    time_remaining: int

    modified_at: datetime
    created_at: datetime

    cards: Optional[List[GameCard]]

    _remove_reverse_relation = field_validator("cards", mode="before")(
        remove_reverse_relation
    )

    @computed_field
    @property
    def time_remaining_clock(self) -> str:
        return f"{(self.time_remaining//60):02}:{(self.time_remaining%60):02}"

    @computed_field
    @property
    def away_team_name_with_default(self) -> str:
        return self.away_team_name or "Away"

    @computed_field
    @property
    def home_team_name_with_default(self) -> str:
        return self.home_team_name or "Home"

    @computed_field
    @property
    def verbose_status(self) -> str:
        return self.status.title().replace("-", " ")


class GameFilterField(enums.EnumStr):
    ID_IN = "id__in"
    STATUS_IN = "status__in"


class GameFilter(BaseModel):
    field: GameFilterField
    value: Union[str, int, List[str], List[int]]


class GameSort(enums.EnumStr):
    ID_ASC = "id"
    ID_DESC = "-id"
    CREATED_AT_ASC = "created_at"
    CREATED_AT_DESC = "-created_at"
    MODIFIED_AT_ASC = "modified_at"
    MODIFIED_AT_DESC = "-modified_at"


class GameResolve(enums.EnumStr):
    CARDS = "cards"


class GameGetOptions(BaseModel):
    resolves: Optional[List[GameResolve]] = []

    _parse_list = field_validator("resolves", mode="before")(parse_list)


class GameQuery(BaseModel, Query):
    filters: List[GameFilter] = []
    sorts: List[GameSort] = [GameSort.ID_ASC]
    resolves: Optional[List[GameResolve]] = []


class GameQueryStringSort(enums.EnumStr):
    ID_ASC = "id"
    ID_DESC = "-id"
    CREATED_AT_ASC = "created_at__id"
    CREATED_AT_DESC = "-created_at__-id"
    MODIFIED_AT_ASC = "modified_at__id"
    MODIFIED_AT_DESC = "-modified_at__-id"


class GameQueryString(BaseModel):
    sort: Optional[GameQueryStringSort] = GameQueryStringSort.MODIFIED_AT_DESC
    id__in: Optional[List[int]] = None
    status__in: Optional[List[GAME_STATUS_VALIDATOR]] = None
    pp: Optional[int] = 10
    p: Optional[int] = 1
    resolves: Optional[List[GameResolve]] = []

    _parse_list = field_validator("id__in", "status__in", "resolves", mode="before")(
        parse_list
    )

    def to_query(self, resolves=None):
        filters = []
        if self.id__in:
            filters.append(GameFilter(field=GameFilterField.ID_IN, value=self.id__in))

        if self.status__in:
            filters.append(
                GameFilter(field=GameFilterField.STATUS_IN, value=self.status__in)
            )

        resolves = resolves or self.resolves
        sorts = self.sort.split("__")
        page_info = PageInfo(num_per_page=self.pp, current_page=self.p)
        return GameQuery(
            filters=filters, sorts=sorts, resolves=resolves, page_info=page_info
        )


class GameResultSet(BaseModel):
    pagination: Pagination
    games: List[Game]
