from datetime import datetime
from typing import List, Optional, Union

from pydantic import StringConstraints, ValidationInfo, computed_field, field_validator
from typing_extensions import Annotated

from score_sphere import enums

from .helpers import NOTSET, BaseModel, parse_list
from .pagination import PageInfo, Pagination
from .query import Query


class MessageCreate(BaseModel):
    text: str


class MessagePatch(BaseModel):
    text: str = NOTSET


class Message(BaseModel):
    id: int

    text: str

    modified_at: datetime
    created_at: datetime


class MessageFilterField(enums.EnumStr):
    ID_IN = "id__in"


class MessageFilter(BaseModel):
    field: MessageFilterField
    value: Union[str, int, List[str], List[int]]


class MessageSort(enums.EnumStr):
    ID_ASC = "id"
    ID_DESC = "-id"
    CREATED_AT_ASC = "created_at"
    CREATED_AT_DESC = "-created_at"
    MODIFIED_AT_ASC = "modified_at"
    MODIFIED_AT_DESC = "-modified_at"


class MessageResolve(enums.EnumStr):
    pass


class MessageGetOptions(BaseModel):
    resolves: Optional[List[MessageResolve]] = []

    _parse_list = field_validator("resolves", mode="before")(parse_list)


class MessageQuery(BaseModel, Query):
    filters: List[MessageFilter] = []
    sorts: List[MessageSort] = [MessageSort.ID_ASC]
    resolves: Optional[List[MessageResolve]] = []


class MessageQueryStringSort(enums.EnumStr):
    ID_ASC = "id"
    ID_DESC = "-id"
    CREATED_AT_ASC = "created_at__id"
    CREATED_AT_DESC = "-created_at__-id"
    MODIFIED_AT_ASC = "modified_at__id"
    MODIFIED_AT_DESC = "-modified_at__-id"


class MessageQueryString(BaseModel):
    sort: Optional[MessageQueryStringSort] = MessageQueryStringSort.MODIFIED_AT_DESC
    id__in: Optional[List[int]] = None
    pp: Optional[int] = 10
    p: Optional[int] = 1
    resolves: Optional[List[MessageResolve]] = []

    _parse_list = field_validator("id__in", "resolves", mode="before")(parse_list)

    def to_query(self, resolves=None):
        filters = []
        if self.id__in:
            filters.append(
                MessageFilter(field=MessageFilterField.ID_IN, value=self.id__in)
            )

        resolves = resolves or self.resolves
        sorts = self.sort.split("__")
        page_info = PageInfo(num_per_page=self.pp, current_page=self.p)
        return MessageQuery(
            filters=filters, sorts=sorts, resolves=resolves, page_info=page_info
        )


class MessageResultSet(BaseModel):
    pagination: Pagination
    messages: List[Message]
