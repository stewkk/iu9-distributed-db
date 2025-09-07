"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import runtime_version as _runtime_version
from google.protobuf import symbol_database as _symbol_database
from google.protobuf.internal import builder as _builder
_runtime_version.ValidateProtobufRuntimeVersion(_runtime_version.Domain.PUBLIC, 5, 29, 0, '', 'api.proto')
_sym_db = _symbol_database.Default()
DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\tapi.proto\x12\x05iu9db"-\n\x0fKeyValueRequest\x12\x0b\n\x03key\x18\x01 \x01(\t\x12\r\n\x05value\x18\x02 \x01(\t"\x19\n\nKeyRequest\x12\x0b\n\x03key\x18\x01 \x01(\t"\x0e\n\x0cEmptyRequest"\x0c\n\nEmptyReply"\x19\n\x08GetReply\x12\r\n\x05value\x18\x01 \x01(\t2\x9a\x01\n\x02Db\x125\n\x06Insert\x12\x16.iu9db.KeyValueRequest\x1a\x11.iu9db.EmptyReply"\x00\x120\n\x06Remove\x12\x11.iu9db.KeyRequest\x1a\x11.iu9db.EmptyReply"\x00\x12+\n\x03Get\x12\x11.iu9db.KeyRequest\x1a\x0f.iu9db.GetReply"\x00')
_globals = globals()
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, _globals)
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'api_pb2', _globals)
if not _descriptor._USE_C_DESCRIPTORS:
    DESCRIPTOR._loaded_options = None
    _globals['_KEYVALUEREQUEST']._serialized_start = 20
    _globals['_KEYVALUEREQUEST']._serialized_end = 65
    _globals['_KEYREQUEST']._serialized_start = 67
    _globals['_KEYREQUEST']._serialized_end = 92
    _globals['_EMPTYREQUEST']._serialized_start = 94
    _globals['_EMPTYREQUEST']._serialized_end = 108
    _globals['_EMPTYREPLY']._serialized_start = 110
    _globals['_EMPTYREPLY']._serialized_end = 122
    _globals['_GETREPLY']._serialized_start = 124
    _globals['_GETREPLY']._serialized_end = 149
    _globals['_DB']._serialized_start = 152
    _globals['_DB']._serialized_end = 306
