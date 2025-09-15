"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import runtime_version as _runtime_version
from google.protobuf import symbol_database as _symbol_database
from google.protobuf.internal import builder as _builder
_runtime_version.ValidateProtobufRuntimeVersion(_runtime_version.Domain.PUBLIC, 5, 29, 0, '', 'api.proto')
_sym_db = _symbol_database.Default()
DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\tapi.proto\x12\x05iu9db"]\n\x0fKeyValueRequest\x12\x0b\n\x03key\x18\x01 \x01(\t\x12\r\n\x05value\x18\x02 \x01(\t\x12\x0f\n\x07version\x18\x03 \x01(\x04\x12\x1d\n\x06source\x18\x04 \x01(\x0e2\r.iu9db.Source"I\n\nKeyRequest\x12\x0b\n\x03key\x18\x01 \x01(\t\x12\x0f\n\x07version\x18\x02 \x01(\x04\x12\x1d\n\x06source\x18\x03 \x01(\x0e2\r.iu9db.Source"\x0e\n\x0cEmptyRequest"\x0c\n\nEmptyReply"7\n\x08GetReply\x12\r\n\x05value\x18\x01 \x01(\t\x12\x0f\n\x07version\x18\x02 \x01(\x04\x12\x0b\n\x03md5\x18\x03 \x01(\x0c"K\n\nGetRequest\x12\x0b\n\x03key\x18\x01 \x01(\t\x12\x1d\n\x06source\x18\x02 \x01(\x0e2\r.iu9db.Source\x12\x11\n\tis_master\x18\x03 \x01(\x08""\n\x12GetMasterNodeReply\x12\x0c\n\x04host\x18\x01 \x01(\t*,\n\x06Source\x12\x11\n\rSOURCE_CLIENT\x10\x00\x12\x0f\n\x0bSOURCE_NODE\x10\x012\xdd\x01\n\x02Db\x125\n\x06Insert\x12\x16.iu9db.KeyValueRequest\x1a\x11.iu9db.EmptyReply"\x00\x120\n\x06Remove\x12\x11.iu9db.KeyRequest\x1a\x11.iu9db.EmptyReply"\x00\x12+\n\x03Get\x12\x11.iu9db.GetRequest\x1a\x0f.iu9db.GetReply"\x00\x12A\n\rGetMasterNode\x12\x13.iu9db.EmptyRequest\x1a\x19.iu9db.GetMasterNodeReply"\x00')
_globals = globals()
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, _globals)
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'api_pb2', _globals)
if not _descriptor._USE_C_DESCRIPTORS:
    DESCRIPTOR._loaded_options = None
    _globals['_SOURCE']._serialized_start = 390
    _globals['_SOURCE']._serialized_end = 434
    _globals['_KEYVALUEREQUEST']._serialized_start = 20
    _globals['_KEYVALUEREQUEST']._serialized_end = 113
    _globals['_KEYREQUEST']._serialized_start = 115
    _globals['_KEYREQUEST']._serialized_end = 188
    _globals['_EMPTYREQUEST']._serialized_start = 190
    _globals['_EMPTYREQUEST']._serialized_end = 204
    _globals['_EMPTYREPLY']._serialized_start = 206
    _globals['_EMPTYREPLY']._serialized_end = 218
    _globals['_GETREPLY']._serialized_start = 220
    _globals['_GETREPLY']._serialized_end = 275
    _globals['_GETREQUEST']._serialized_start = 277
    _globals['_GETREQUEST']._serialized_end = 352
    _globals['_GETMASTERNODEREPLY']._serialized_start = 354
    _globals['_GETMASTERNODEREPLY']._serialized_end = 388
    _globals['_DB']._serialized_start = 437
    _globals['_DB']._serialized_end = 658
