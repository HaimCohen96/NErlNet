from collections import OrderedDict

ARGS_TYPE = 0
VALUE_TYPE = 1
IP_TYPE = 2
PORT_TYPE = 3
POLICY_TYPE = 4
DEVICE_TYPE = 5
ROUTER_TYPE = 6
CLIENT_TYPE = 7
SOURCE_TYPE = 8
WORKER_TYPE = 9
MAIN_SERVER_TYPE = 10
API_SERVER_TYPE = 11
SPECIAL_ENTITY_TYPE = 12
NONE_TYPE = -1

RouterPolicyDict = {
    "Shortest-Path":0,
    "Routing-Table":1
}

SensorPolicyDict = {
    "Casting":1,
    "Round-Robin":2
}