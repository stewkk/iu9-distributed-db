"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import runtime_version as _runtime_version
from google.protobuf import symbol_database as _symbol_database
from google.protobuf.internal import builder as _builder
_runtime_version.ValidateProtobufRuntimeVersion(_runtime_version.Domain.PUBLIC, 5, 29, 0, '', 'api.proto')
_sym_db = _symbol_database.Default()
DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\tapi.proto"-\n\x0fKeyValueRequest\x12\x0b\n\x03key\x18\x01 \x01(\t\x12\r\n\x05value\x18\x02 \x01(\t"\x19\n\nKeyRequest\x12\x0b\n\x03key\x18\x01 \x01(\t"\x0c\n\nEmptyReply"\x19\n\x08GetReply\x12\r\n\x05value\x18\x01 \x01(\t2\xa1\x01\n\x02Db\x12)\n\x06Insert\x12\x10.KeyValueRequest\x1a\x0b.EmptyReply"\x00\x12)\n\x06Update\x12\x10.KeyValueRequest\x1a\x0b.EmptyReply"\x00\x12$\n\x06Remove\x12\x0b.KeyRequest\x1a\x0b.EmptyReply"\x00\x12\x1f\n\x03Get\x12\x0b.KeyRequest\x1a\t.GetReply"\x00')
_globals = globals()
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, _globals)
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'api_pb2', _globals)
if not _descriptor._USE_C_DESCRIPTORS:
    DESCRIPTOR._loaded_options = None
    _globals['_KEYVALUEREQUEST']._serialized_start = 13
    _globals['_KEYVALUEREQUEST']._serialized_end = 58
    _globals['_KEYREQUEST']._serialized_start = 60
    _globals['_KEYREQUEST']._serialized_end = 85
    _globals['_EMPTYREPLY']._serialized_start = 87
    _globals['_EMPTYREPLY']._serialized_end = 99
    _globals['_GETREPLY']._serialized_start = 101
    _globals['_GETREPLY']._serialized_end = 126
    _globals['_DB']._serialized_start = 129
    _globals['_DB']._serialized_end = 290
