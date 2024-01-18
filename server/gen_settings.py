#### @martysama0134 start scripts ####
from platform import system as p_system
v_system = p_system()

from subprocess import check_output as sp_co, call as sp_call, CalledProcessError as sp_CalledProcessError
def fShell(szCmd, bRet=False):
	try:
		if bRet:
			return sp_co(szCmd, shell=True)[:-1]	# remove final \n
		else:
			return sp_call(szCmd, shell=True)
	except sp_CalledProcessError:
		return -1

if v_system=="FreeBSD":
	v_adminPageLocalIP=fShell("ifconfig ix0 | grep -Eo 'inet ([0-9]{1,3}\.){3}([0-9]{1,3})' | awk '{print $2}'", True)
elif v_system=="Linux":
	v_adminPageLocalIP=fShell("ifconfig eth0 | grep -Eo 'inet addr:([0-9]{1,3}\.){3}([0-9]{1,3})' | awk -F':' '{print $2}'", True)
elif v_system=="Windows":
	v_adminPageLocalIP="localhost"

v_adminPagePassword='AFSJ423JnkLALJKA'						#adminpage_password
v_serverHostname='127.0.0.1'								#host for sql connections
v_serverUserPass='mt2!user @mt2!pass'						#user&pwd for sql connection
v_serverData="%s %s" % (v_serverHostname, v_serverUserPass)	#host, user and pwd for db sql connections
v_dbHostname='127.0.0.1'									#default hostname for db
v_dbPort=30051												#default port for db (the others will be automatically calculated)
v_mysqlport=3306											#default mysql port (3306 or 0)
v_logFolder='logs'											#name of the all_log path
v_chanFolder='chan/'										#name of the channel path
v_chanPath='../'											#workaround that should be equivalent to $v_charS paths per ../

M2SD_SRV1 = {
	"account" : "account",
	"common" : "common",
	"log" : "log",
	"player" : "player",
}

class M2TYPE:
	SERVER, DB, AUTH1, AUTH2, AUTH3, AUTH4, AUTH5, AUTH6, CHANFOLDER, CHANNEL, CORE = xrange(11)
	NOCHAN = 0
	NOCHAN1, NOCHAN2, NOCHAN3, NOCHAN4, NOCHAN5, NOCHAN6 = xrange(6)

class PORT:
	RANDOMI = v_dbPort
	RANDOM = 0
	PORT, P2P_PORT, DB_PORT, BIND_PORT = xrange(4)
	lPORT = ("PORT", "P2P_PORT", "DB_PORT", "BIND_PORT")

M2CONFIG_SRV1 = {
	"db": {
		"general": (
			("SQL_ACCOUNT = \\\"%s %s %s %d\\\"", (v_serverHostname, M2SD_SRV1["account"], v_serverUserPass, v_mysqlport)),
			("SQL_COMMON = \\\"%s %s %s %d\\\"", (v_serverHostname, M2SD_SRV1["common"], v_serverUserPass, v_mysqlport)),
			("SQL_PLAYER = \\\"%s %s %s %d\\\"", (v_serverHostname, M2SD_SRV1["player"], v_serverUserPass, v_mysqlport)),
			#
			("TABLE_POSTFIX = \\\"%s\\\"", ("")),
			#
			# ("BIND_PORT = %s", (v_dbPort,)),
			# ("DB_SLEEP_MSEC = 10", ()),
			("CLIENT_HEART_FPS = %u", (25)),
			# ("HASH_PLAYER_LIFE_SEC = %u", (600)),
			("PLAYER_ID_START = %u", (100)),
			("PLAYER_DELETE_LEVEL_LIMIT = %u", (105)),
			("PLAYER_DELETE_LEVEL_LIMIT_LOWER = %u", (1)),
			# ("PLAYER_DELETE_LEVEL_LIMIT_LOWER = %u", (15)),
			("ITEM_ID_RANGE = %u %u ", (100000000, 200000000)),
			# ("BACKUP_LIMIT_SEC = %u", (3600)),
			("LOCALE = %s", ("latin1")),
			("TEST_SERVER = %d", (False)),
			("LOG = %d", (False)),
		),
		"extra": (
			("PROTO_FROM_DB = %u", (True)),
			("MIRROR2DB = %u", (False)),
		)
	},
	"core": {
		M2TYPE.AUTH1: (
			("AUTH_SERVER: %s", ("master")),
			("PLAYER_SQL: %s %s %d", (v_serverData, M2SD_SRV1["account"], v_mysqlport)),
			("define_client_version: %s", ("523759847")),
		),
		M2TYPE.AUTH2: (
			("AUTH_SERVER: %s", ("master")),
			("PLAYER_SQL: %s %s %d", (v_serverData, M2SD_SRV1["account"], v_mysqlport)),
			("define_client_version: %s", ("523759847")),
		),
		M2TYPE.AUTH3: (
			("AUTH_SERVER: %s", ("master")),
			("PLAYER_SQL: %s %s %d", (v_serverData, M2SD_SRV1["account"], v_mysqlport)),
			("define_client_version: %s", ("523759847")),
		),
		M2TYPE.AUTH4: (
			("AUTH_SERVER: %s", ("master")),
			("PLAYER_SQL: %s %s %d", (v_serverData, M2SD_SRV1["account"], v_mysqlport)),
			("define_client_version: %s", ("523759847")),
		),
		M2TYPE.AUTH5: (
			("AUTH_SERVER: %s", ("master")),
			("PLAYER_SQL: %s %s %d", (v_serverData, M2SD_SRV1["account"], v_mysqlport)),
			("define_client_version: %s", ("523759847")),
		),
		M2TYPE.AUTH6: (
			("AUTH_SERVER: %s", ("master")),
			("PLAYER_SQL: %s %s %d", (v_serverData, M2SD_SRV1["account"], v_mysqlport)),
			("define_client_version: %s", ("523759847")),
		),
		M2TYPE.CORE: (
			("PLAYER_SQL: %s %s %d", (v_serverData, M2SD_SRV1["player"], v_mysqlport)),
		),
		"general": (
			#("BIND_IP: %s", ("localhost")),
			# ("TABLE_POSTFIX: %s", ("")),
			# ("ITEM_ID_RANGE: %u %u", (5000001, 10000000)),
			("VIEW_RANGE: %u", (10000)),
			("PASSES_PER_SEC: %u", (25)),
			("SAVE_EVENT_SECOND_CYCLE: %u", (180)),
			("PING_EVENT_SECOND_CYCLE: %u", (180)),
			#
			("DB_ADDR: %s", (v_dbHostname)),
			("COMMON_SQL: %s %s %d", (v_serverData, M2SD_SRV1["common"], v_mysqlport)),
			("LOG_SQL: %s %s %d", (v_serverData, M2SD_SRV1["log"], v_mysqlport)),
			("TEST_SERVER: %d", (False)),
			("PK_SERVER: %d", (True)),
			("ADMINPAGE_IP1: %s", (v_adminPageLocalIP)),
			("ADMINPAGE_PASSWORD: %s", (v_adminPagePassword)),
			("MAX_LEVEL: %u", (120)),
		),
		"extra": (
			# ("CHECK_VERSION_SERVER: %u", (True)),
			# ("CHECK_VERSION_VALUE: %u", (1215955205)),
			("CHANGE_ATTR_TIME_LIMIT: %u", (False)),
			("EMOTION_MASK_REQUIRE: %u", (False)),
			("PRISM_ITEM_REQUIRE: %u", (False)),
			("SHOP_PRICE_3X_TAX: %u", (False)),
			("GLOBAL_SHOUT: %u", (True)),
			("ITEM_COUNT_LIMIT: %u", (50000)),
			("STATUS_POINT_GET_LEVEL_LIMIT: %u", (105)),
			("STATUS_POINT_SET_MAX_VALUE: %u", (90)),
			("SHOUT_LIMIT_LEVEL: %u", (15)),
			("DB_LOG_LEVEL: %u", (1)),
			("EMPIRE_LANGUAGE_CHECK: %u", (False)),
			# ("ITEM_DESTROY_TIME_AUTOGIVE: %u", (30)),
			# ("ITEM_DESTROY_TIME_DROPITEM: %u", (30)),
			# ("ITEM_DESTROY_TIME_DROPGOLD: %u", (30)),
		),
	},
}

COMMONCHAN=(
	{
		"name": "core1",
		"type": M2TYPE.CORE,
		"port": PORT.RANDOM,
		"p2p_port": PORT.RANDOM,
		"config": M2CONFIG_SRV1["core"],
		"maps" : "67 68 69 71 104",
	},
	{
		"name": "core2",
		"type": M2TYPE.CORE,
		"port": PORT.RANDOM,
		"p2p_port": PORT.RANDOM,
		"config": M2CONFIG_SRV1["core"],
		"maps" : "3 23 43 63 65 216 66 70 72 64 355 27 156 157",
	},
	{
		"name": "core3",
		"type": M2TYPE.CORE,
		"port": PORT.RANDOM,
		"p2p_port": PORT.RANDOM,
		"config": M2CONFIG_SRV1["core"],
		"maps" : "301 302 209 353 303 304 74 151 357",
	},
	{
		"name": "core4",
		"type": M2TYPE.CORE,
		"port": PORT.RANDOM,
		"p2p_port": PORT.RANDOM,
		"config": M2CONFIG_SRV1["core"],
		"maps" : "226 61 25 26 352 62 212 351 218 354 231 232 233",
	},
	{
		"name": "core5",
		"type": M2TYPE.CORE,
		"port": PORT.RANDOM,
		"p2p_port": PORT.RANDOM,
		"config": M2CONFIG_SRV1["core"],
		"maps" : "210 219 217 208 73 313 314 315 135 136",
	},
)

CHAN99=(
	{
		"name": "core1",
		"type": M2TYPE.CORE,
		"port": PORT.RANDOM,
		"p2p_port": PORT.RANDOM,
		"config": M2CONFIG_SRV1["core"],
		"maps" : "30 156 157",
	},
	{
		"name": "core2",
		"type": M2TYPE.CORE,
		"port": PORT.RANDOM,
		"p2p_port": PORT.RANDOM,
		"config": M2CONFIG_SRV1["core"],
		"maps" : "81 103 105 110 111 356 113 221",
	},
	{
		"name": "core3",
		"type": M2TYPE.CORE,
		"port": PORT.RANDOM,
		"p2p_port": PORT.RANDOM,
		"config": M2CONFIG_SRV1["core"],
		"maps" : "1",
	},
	{
		"name": "core4",
		"type": M2TYPE.CORE,
		"port": PORT.RANDOM,
		"p2p_port": PORT.RANDOM,
		"config": M2CONFIG_SRV1["core"],
		"maps" : "21",
	},
	{
		"name": "core5",
		"type": M2TYPE.CORE,
		"port": PORT.RANDOM,
		"p2p_port": PORT.RANDOM,
		"config": M2CONFIG_SRV1["core"],
		"maps" : "41",
	},
)

M2S=(
	{
		"name": "srv1",
		"type": M2TYPE.SERVER,
		"isextra": True,
		"sub": (
			{
				"name": "db",
				"type": M2TYPE.DB,
				"port": PORT.RANDOM,
				"config": M2CONFIG_SRV1["db"],
			},
			{
				"name": "auth1",
				"type": M2TYPE.AUTH1,
				"port": PORT.RANDOM,
				"p2p_port": PORT.RANDOM,
				"config": M2CONFIG_SRV1["core"],
			},
			{
				"name": "auth2",
				"type": M2TYPE.AUTH2,
				"port": PORT.RANDOM,
				"p2p_port": PORT.RANDOM,
				"config": M2CONFIG_SRV1["core"],
			},
			{
				"name": "auth3",
				"type": M2TYPE.AUTH3,
				"port": PORT.RANDOM,
				"p2p_port": PORT.RANDOM,
				"config": M2CONFIG_SRV1["core"],
			},
			{
				"name": "auth4",
				"type": M2TYPE.AUTH4,
				"port": PORT.RANDOM,
				"p2p_port": PORT.RANDOM,
				"config": M2CONFIG_SRV1["core"],
			},
			{
				"name": "auth5",
				"type": M2TYPE.AUTH5,
				"port": PORT.RANDOM,
				"p2p_port": PORT.RANDOM,
				"config": M2CONFIG_SRV1["core"],
			},
			{
				"name": "auth6",
				"type": M2TYPE.AUTH6,
				"port": PORT.RANDOM,
				"p2p_port": PORT.RANDOM,
				"config": M2CONFIG_SRV1["core"],
			},
			{
				"name": "chan",
				"type": M2TYPE.CHANFOLDER,
				"sub": (
					{
						"name": "ch1",
						"type": M2TYPE.CHANNEL,
						"chan": 1,
						"sub": COMMONCHAN,
					},
					{
						"name": "ch2",
						"type": M2TYPE.CHANNEL,
						"chan": 2,
						"sub": COMMONCHAN,
					},
					{
						"name": "ch3",
						"type": M2TYPE.CHANNEL,
						"chan": 3,
						"sub": COMMONCHAN,
					},
					{
						"name": "ch4",
						"type": M2TYPE.CHANNEL,
						"chan": 4,
						"sub": COMMONCHAN,
					},
					{
						"name": "ch5",
						"type": M2TYPE.CHANNEL,
						"chan": 5,
						"sub": COMMONCHAN,
					},
					{
						"name": "ch6",
						"type": M2TYPE.CHANNEL,
						"chan": 6,
						"sub": COMMONCHAN,
					},
					{
						"name": "ch99",
						"type": M2TYPE.CHANNEL,
						"chan": 99,
						"sub": CHAN99,
					},
				)
			}
		)
	},
)

CustIpfwList="""#!/bin/sh
IPF="ipfw -q add"
ipfw -q -f flush

#loopback
$IPF 10 allow all from any to any via lo0
$IPF 20 deny all from any to 127.0.0.0/8
$IPF 30 deny all from 127.0.0.0/8 to any
$IPF 40 deny tcp from any to any frag

# stateful
$IPF 50 check-state
$IPF 60 allow tcp from any to any established
$IPF 70 allow all from any to any out keep-state
$IPF 80 deny icmp from any to any

# open port ftp (20, 21), ssh (8752), mail (25)
# http (80), https (443), dns (53), mysql (3306)
default_udp_yes_ports='53'
default_tcp_yes_ports='8752 53 3306 80 443 51566'
default_tcp_no_ports=''

# here auth PORTs for "NORM"/"..." thing
metin2_udp_yes_ports='%s'
# here PORTs
metin2_tcp_yes_ports='%s'
# here DB_PORTs and P2P_PORTs
metin2_tcp_no_ports='%s'

# merge lists
udp_yes_ports="$default_udp_yes_ports $metin2_udp_yes_ports"
tcp_yes_ports="$default_tcp_yes_ports $metin2_tcp_yes_ports"
tcp_nop_ports="$default_tcp_no_ports $metin2_tcp_no_ports"

# white ip list
white_sites=''

# block tcp/udp ports
for val in $tcp_nop_ports; do
	$IPF 2220 allow all from 127.0.0.0/8 to any $val
	for whitez in $white_sites; do
		$IPF 2210 allow tcp from $whitez to any $val in
		$IPF 2210 allow tcp from 127.0.0.0/8 to $whitez $val out
	done
	$IPF 2230 deny all from any to me $val
done
# unblock tcp ports
for val in $tcp_yes_ports; do
	$IPF 2200 allow tcp from any to any $val in limit src-addr 20
	$IPF 2210 allow tcp from any to any $val out
done
# unblock udp ports
for val in $udp_yes_ports; do
	$IPF 2200 allow udp from any to any $val in limit src-addr 20
	$IPF 2210 allow udp from any to any $val out
done
"""
