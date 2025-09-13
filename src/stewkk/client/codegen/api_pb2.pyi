from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Optional as _Optional, Union as _Union
DESCRIPTOR: _descriptor.FileDescriptor

class Source(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = ()
    SOURCE_CLIENT: _ClassVar[Source]
    SOURCE_NODE: _ClassVar[Source]
SOURCE_CLIENT: Source
SOURCE_NODE: Source

class KeyValueRequest(_message.Message):
    __slots__ = ('key', 'value', 'version', 'source')
    KEY_FIELD_NUMBER: _ClassVar[int]
    VALUE_FIELD_NUMBER: _ClassVar[int]
    VERSION_FIELD_NUMBER: _ClassVar[int]
    SOURCE_FIELD_NUMBER: _ClassVar[int]
    key: str
    value: str
    version: int
    source: Source

    def __init__(self, key: _Optional[str]=..., value: _Optional[str]=..., version: _Optional[int]=..., source: _Optional[_Union[Source, str]]=...) -> None:
        ...

class KeyRequest(_message.Message):
    __slots__ = ('key', 'version', 'source')
    KEY_FIELD_NUMBER: _ClassVar[int]
    VERSION_FIELD_NUMBER: _ClassVar[int]
    SOURCE_FIELD_NUMBER: _ClassVar[int]
    key: str
    version: int
    source: Source

    def __init__(self, key: _Optional[str]=..., version: _Optional[int]=..., source: _Optional[_Union[Source, str]]=...) -> None:
        ...

class EmptyRequest(_message.Message):
    __slots__ = ()

    def __init__(self) -> None:
        ...

class EmptyReply(_message.Message):
    __slots__ = ()

    def __init__(self) -> None:
        ...

class GetReply(_message.Message):
    __slots__ = ('value', 'version', 'md5')
    VALUE_FIELD_NUMBER: _ClassVar[int]
    VERSION_FIELD_NUMBER: _ClassVar[int]
    MD5_FIELD_NUMBER: _ClassVar[int]
    value: str
    version: int
    md5: bytes

    def __init__(self, value: _Optional[str]=..., version: _Optional[int]=..., md5: _Optional[bytes]=...) -> None:
        ...

class GetRequest(_message.Message):
    __slots__ = ('key', 'source', 'is_master')
    KEY_FIELD_NUMBER: _ClassVar[int]
    SOURCE_FIELD_NUMBER: _ClassVar[int]
    IS_MASTER_FIELD_NUMBER: _ClassVar[int]
    key: str
    source: Source
    is_master: bool

    def __init__(self, key: _Optional[str]=..., source: _Optional[_Union[Source, str]]=..., is_master: bool=...) -> None:
        ...
