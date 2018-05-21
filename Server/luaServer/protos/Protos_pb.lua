-- Generated By protoc-gen-lua Do not Edit
local protobuf = require "protobuf"
module('Protos_pb')


USERINFO = protobuf.Descriptor();
local USERINFO_NAME_FIELD = protobuf.FieldDescriptor();
local USERINFO_SCORE_FIELD = protobuf.FieldDescriptor();
DATARESP = protobuf.Descriptor();
local DATARESP_USERS_FIELD = protobuf.FieldDescriptor();

USERINFO_NAME_FIELD.name = "name"
USERINFO_NAME_FIELD.full_name = ".deepdf.UserInfo.name"
USERINFO_NAME_FIELD.number = 1
USERINFO_NAME_FIELD.index = 0
USERINFO_NAME_FIELD.label = 1
USERINFO_NAME_FIELD.has_default_value = false
USERINFO_NAME_FIELD.default_value = ""
USERINFO_NAME_FIELD.type = 9
USERINFO_NAME_FIELD.cpp_type = 9

USERINFO_SCORE_FIELD.name = "score"
USERINFO_SCORE_FIELD.full_name = ".deepdf.UserInfo.score"
USERINFO_SCORE_FIELD.number = 2
USERINFO_SCORE_FIELD.index = 1
USERINFO_SCORE_FIELD.label = 1
USERINFO_SCORE_FIELD.has_default_value = false
USERINFO_SCORE_FIELD.default_value = 0
USERINFO_SCORE_FIELD.type = 13
USERINFO_SCORE_FIELD.cpp_type = 3

USERINFO.name = "UserInfo"
USERINFO.full_name = ".deepdf.UserInfo"
USERINFO.nested_types = {}
USERINFO.enum_types = {}
USERINFO.fields = {USERINFO_NAME_FIELD, USERINFO_SCORE_FIELD}
USERINFO.is_extendable = false
USERINFO.extensions = {}
DATARESP_USERS_FIELD.name = "users"
DATARESP_USERS_FIELD.full_name = ".deepdf.DataResp.users"
DATARESP_USERS_FIELD.number = 1
DATARESP_USERS_FIELD.index = 0
DATARESP_USERS_FIELD.label = 3
DATARESP_USERS_FIELD.has_default_value = false
DATARESP_USERS_FIELD.default_value = {}
DATARESP_USERS_FIELD.message_type = USERINFO
DATARESP_USERS_FIELD.type = 11
DATARESP_USERS_FIELD.cpp_type = 10

DATARESP.name = "DataResp"
DATARESP.full_name = ".deepdf.DataResp"
DATARESP.nested_types = {}
DATARESP.enum_types = {}
DATARESP.fields = {DATARESP_USERS_FIELD}
DATARESP.is_extendable = false
DATARESP.extensions = {}

DataResp = protobuf.Message(DATARESP)
UserInfo = protobuf.Message(USERINFO)
