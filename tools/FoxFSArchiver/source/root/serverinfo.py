import app
import localeinfo

app.ServerName = None

if __USE_DISTRIBUTE__:
	SRV1 = {
		"name":"KEYNES2",
		"host":"92.246.91.31",
		"auth" : [30052, 30054, 30056, 30058, 30060, 30062],
		"ch1":[30064, 30066, 30068, 30070, 30072],
		"ch2":[30074, 30076, 30078, 30080, 30082],
		"ch3":[30084, 30086, 30088, 30090, 30092],
		"ch4":[30094, 30096, 30098, 30100, 30102],
		"ch5":[30104, 30106, 30108, 30110, 30112],
	}
else:
	SRV1 = {
		"name":"KEYNES2",
		"host":"92.246.89.188",
		"auth" : [30052, 30054, 30056, 30058, 30060, 30062],
		"ch1":[30064, 30066, 30068, 30070, 30072],
		"ch2":[30074, 30076, 30078, 30080, 30082],
		"ch3":[30084, 30086, 30088, 30090, 30092],
		"ch4":[30094, 30096, 30098, 30100, 30102],
		"ch5":[30104, 30106, 30108, 30110, 30112],
	}

STATE_NONE = "...."

STATE_DICT = {
	0 : STATE_NONE,
	1 : "Normal",
	2 : "Busy",
	3 : "Full"
}

SERVER1_CHANNEL_DICT = {
	1 : {"key":11,"name":"CH 1","ip":SRV1["host"],"tcp_port":SRV1["ch1"][0],"udp_port":SRV1["ch1"][0],"state":STATE_NONE,},
	2 : {"key":12,"name":"CH 2","ip":SRV1["host"],"tcp_port":SRV1["ch2"][0],"udp_port":SRV1["ch2"][0],"state":STATE_NONE,},
	3 : {"key":13,"name":"CH 3","ip":SRV1["host"],"tcp_port":SRV1["ch3"][0],"udp_port":SRV1["ch3"][0],"state":STATE_NONE,},
	4 : {"key":14,"name":"CH 4","ip":SRV1["host"],"tcp_port":SRV1["ch4"][0],"udp_port":SRV1["ch4"][0],"state":STATE_NONE,},
	5 : {"key":15,"name":"CH 5","ip":SRV1["host"],"tcp_port":SRV1["ch5"][0],"udp_port":SRV1["ch5"][0],"state":STATE_NONE,},
}

REGION_NAME_DICT = {
	0 : SRV1["name"],
}

REGION_AUTH_SERVER_DICT = {
	0 : {
		1 : { "ip":SRV1["host"], "port":SRV1["auth"][0], },
		2 : { "ip":SRV1["host"], "port":SRV1["auth"][1], },
		3 : { "ip":SRV1["host"], "port":SRV1["auth"][2], },
		4 : { "ip":SRV1["host"], "port":SRV1["auth"][3], },
		5 : { "ip":SRV1["host"], "port":SRV1["auth"][4], },
	}
}

REGION_DICT = {
	0 : {
		1 : { "name" :SRV1["name"], "channel" : SERVER1_CHANNEL_DICT, },
	},
}

MARKADDR_DICT = {
	10 : { "ip" : SRV1["host"], "tcp_port" : SRV1["ch1"], "mark" : "mark.tga", "symbol_path" : "", },
}

TESTADDR = { "ip" : SRV1["host"], "tcp_port" : SRV1["ch1"], "udp_port" : SRV1["ch1"], }
