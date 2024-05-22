from tortoise import Model, fields

from .helpers import TimestampMixin


class Message(TimestampMixin, Model):
    id = fields.IntField(pk=True)

    text = fields.TextField()
