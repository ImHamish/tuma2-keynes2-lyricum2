SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for acce
-- ----------------------------
DROP TABLE IF EXISTS `acce`;
CREATE TABLE `acce`  (
  `pid` int(11) NOT NULL,
  `time` timestamp(0) NOT NULL DEFAULT '0000-00-00 00:00:00',
  `x` int(11) NOT NULL,
  `y` int(11) NOT NULL,
  `item_vnum` int(11) NOT NULL,
  `item_uid` int(11) NOT NULL,
  `item_count` smallint(6) NOT NULL,
  `item_abs_chance` smallint(6) NOT NULL,
  `success` varbinary(3) NOT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_cs ROW_FORMAT = Fixed;

-- ----------------------------
-- Table structure for attr_transfer
-- ----------------------------
DROP TABLE IF EXISTS `attr_transfer`;
CREATE TABLE `attr_transfer`  (
  `pid` int(11) NOT NULL,
  `time` timestamp(0) NOT NULL DEFAULT '0000-00-00 00:00:00',
  `x` int(11) NOT NULL,
  `y` int(11) NOT NULL,
  `item_vnum` int(11) NOT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_cs ROW_FORMAT = Fixed;

-- ----------------------------
-- Table structure for bootlog
-- ----------------------------
DROP TABLE IF EXISTS `bootlog`;
CREATE TABLE `bootlog`  (
  `time` datetime(0) NULL DEFAULT NULL,
  `hostname` varbinary(56) NULL DEFAULT NULL,
  `channel` int(11) NULL DEFAULT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for change_empire
-- ----------------------------
DROP TABLE IF EXISTS `change_empire`;
CREATE TABLE `change_empire`  (
  `account_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `change_count` int(11) NULL DEFAULT NULL,
  `data` datetime(0) NULL DEFAULT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Fixed;

-- ----------------------------
-- Table structure for change_name
-- ----------------------------
DROP TABLE IF EXISTS `change_name`;
CREATE TABLE `change_name`  (
  `pid` int(10) UNSIGNED NULL DEFAULT NULL,
  `old_name` varbinary(16) NULL DEFAULT NULL,
  `new_name` varbinary(16) NULL DEFAULT NULL,
  `time` datetime(0) NULL DEFAULT NULL,
  `ip` varbinary(15) NULL DEFAULT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for chat_log
-- ----------------------------
DROP TABLE IF EXISTS `chat_log`;
CREATE TABLE `chat_log`  (
  `where` int(10) UNSIGNED NOT NULL,
  `who_id` int(10) UNSIGNED NOT NULL,
  `who_name` varchar(25) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `whom_id` int(10) UNSIGNED NULL DEFAULT NULL,
  `whom_name` varchar(25) CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT NULL,
  `type` enum('NORMAL','WHISPER','PARTY','GUILD') CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  `msg` varbinary(512) NOT NULL,
  `when` datetime(0) NOT NULL DEFAULT current_timestamp,
  `ip` varchar(15) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for command_log
-- ----------------------------
DROP TABLE IF EXISTS `command_log`;
CREATE TABLE `command_log`  (
  `userid` int(10) UNSIGNED NULL DEFAULT NULL,
  `server` int(11) NULL DEFAULT 999,
  `ip` varbinary(15) NULL DEFAULT NULL,
  `port` int(10) UNSIGNED NULL DEFAULT NULL,
  `username` varbinary(16) NULL DEFAULT NULL,
  `command` varbinary(300) NULL DEFAULT NULL,
  `date` datetime(0) NULL DEFAULT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for cube
-- ----------------------------
DROP TABLE IF EXISTS `cube`;
CREATE TABLE `cube`  (
  `pid` int(10) UNSIGNED NULL DEFAULT NULL,
  `time` datetime(0) NULL DEFAULT NULL,
  `x` int(10) UNSIGNED NULL DEFAULT NULL,
  `y` int(10) UNSIGNED NULL DEFAULT NULL,
  `item_vnum` int(10) UNSIGNED NULL DEFAULT NULL,
  `item_uid` int(10) UNSIGNED NULL DEFAULT NULL,
  `item_count` int(11) NULL DEFAULT NULL,
  `success` int(11) NULL DEFAULT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Fixed;

-- ----------------------------
-- Table structure for dragon_slay_log
-- ----------------------------
DROP TABLE IF EXISTS `dragon_slay_log`;
CREATE TABLE `dragon_slay_log`  (
  `guild_id` int(10) UNSIGNED NOT NULL,
  `vnum` int(10) UNSIGNED NOT NULL,
  `start_time` datetime(0) NOT NULL DEFAULT current_timestamp,
  `end_time` datetime(0) NOT NULL DEFAULT current_timestamp
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for fish_log
-- ----------------------------
DROP TABLE IF EXISTS `fish_log`;
CREATE TABLE `fish_log`  (
  `time` datetime(0) NOT NULL DEFAULT current_timestamp,
  `player_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `map_index` tinyint(4) NOT NULL DEFAULT 0,
  `fish_id` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `fishing_level` int(11) NOT NULL DEFAULT 0,
  `waiting_time` int(11) NOT NULL DEFAULT 0,
  `success` tinyint(4) NOT NULL DEFAULT 0,
  `size` smallint(6) NOT NULL DEFAULT 0
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Fixed;

-- ----------------------------
-- Table structure for gametimelog
-- ----------------------------
DROP TABLE IF EXISTS `gametimelog`;
CREATE TABLE `gametimelog`  (
  `login` varbinary(16) NULL DEFAULT NULL,
  `type` int(10) UNSIGNED NULL DEFAULT NULL,
  `logon_time` datetime(0) NULL DEFAULT NULL,
  `logout_time` datetime(0) NULL DEFAULT NULL,
  `use_time` int(10) UNSIGNED NULL DEFAULT NULL,
  `ip` varbinary(15) NULL DEFAULT NULL,
  `server` varbinary(56) NULL DEFAULT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for goldlog
-- ----------------------------
DROP TABLE IF EXISTS `goldlog`;
CREATE TABLE `goldlog`  (
  `date` date NULL DEFAULT NULL,
  `time` time(0) NULL DEFAULT NULL,
  `pid` int(10) UNSIGNED NULL DEFAULT NULL,
  `what` int(10) UNSIGNED NULL DEFAULT NULL,
  `how` varbinary(33) NULL DEFAULT NULL COMMENT 'contains: QUEST, BUY, SELL, SHOP_BUY, SHOP_SELL, EXCHANGE_TAKE, EXCHANGE_GIVE',
  `hint` varbinary(80) NULL DEFAULT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for hack_crc_log
-- ----------------------------
DROP TABLE IF EXISTS `hack_crc_log`;
CREATE TABLE `hack_crc_log`  (
  `time` datetime(0) NULL DEFAULT NULL,
  `login` varbinary(16) NULL DEFAULT NULL,
  `name` varbinary(16) NULL DEFAULT NULL,
  `ip` varbinary(15) NULL DEFAULT NULL,
  `server` varbinary(56) NULL DEFAULT NULL,
  `why` varbinary(33) NULL DEFAULT NULL,
  `crc` int(10) UNSIGNED NULL DEFAULT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for hack_log
-- ----------------------------
DROP TABLE IF EXISTS `hack_log`;
CREATE TABLE `hack_log`  (
  `time` datetime(0) NULL DEFAULT NULL,
  `login` varbinary(16) NULL DEFAULT NULL,
  `name` varbinary(16) NULL DEFAULT NULL,
  `ip` varbinary(15) NULL DEFAULT NULL,
  `server` varbinary(56) NULL DEFAULT NULL,
  `why` varbinary(128) NULL DEFAULT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for hack_report
-- ----------------------------
DROP TABLE IF EXISTS `hack_report`;
CREATE TABLE `hack_report`  (
  `pid` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `name` varchar(24) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL DEFAULT 'NONAME',
  `ip` varchar(15) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL DEFAULT 'NO_IP',
  `reason` varchar(48) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL DEFAULT 'NOREASON',
  `when` datetime(0) NOT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Fixed;

-- ----------------------------
-- Table structure for hackshield_log
-- ----------------------------
DROP TABLE IF EXISTS `hackshield_log`;
CREATE TABLE `hackshield_log`  (
  `time` datetime(0) NULL DEFAULT NULL,
  `account_id` int(10) UNSIGNED NULL DEFAULT NULL,
  `login` varbinary(16) NULL DEFAULT NULL,
  `pid` int(10) UNSIGNED NULL DEFAULT NULL,
  `name` varbinary(16) NULL DEFAULT NULL,
  `reason` int(10) UNSIGNED NULL DEFAULT NULL,
  `ip` int(10) UNSIGNED NULL DEFAULT NULL COMMENT 'inet_aton(\'%s\')'
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for invalid_server_log
-- ----------------------------
DROP TABLE IF EXISTS `invalid_server_log`;
CREATE TABLE `invalid_server_log`  (
  `locale_type` int(10) UNSIGNED NULL DEFAULT NULL,
  `log_date` datetime(0) NULL DEFAULT NULL,
  `ip` varbinary(15) NULL DEFAULT NULL,
  `revision` varbinary(16) NULL DEFAULT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for levellog
-- ----------------------------
DROP TABLE IF EXISTS `levellog`;
CREATE TABLE `levellog`  (
  `name` varbinary(16) NULL DEFAULT NULL,
  `level` int(10) UNSIGNED NULL DEFAULT NULL,
  `time` datetime(0) NULL DEFAULT NULL,
  `account_id` int(10) UNSIGNED NULL DEFAULT NULL,
  `pid` int(10) UNSIGNED NULL DEFAULT NULL COMMENT 'contains REPLACE query!',
  `playtime` int(11) NULL DEFAULT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for log
-- ----------------------------
DROP TABLE IF EXISTS `log`;
CREATE TABLE `log`  (
  `type` varbinary(20) NULL DEFAULT NULL COMMENT 'contains: CHARACTER, ITEM',
  `time` datetime(0) NULL DEFAULT NULL,
  `who` int(10) UNSIGNED NULL DEFAULT NULL,
  `x` int(10) UNSIGNED NULL DEFAULT NULL,
  `y` int(10) UNSIGNED NULL DEFAULT NULL,
  `what` bigint(20) UNSIGNED NULL DEFAULT NULL,
  `how` varbinary(50) NULL DEFAULT NULL,
  `hint` varbinary(80) NULL DEFAULT NULL COMMENT 'snprintf with 80u size',
  `ip` varbinary(15) NULL DEFAULT NULL,
  `vnum` int(10) UNSIGNED NULL DEFAULT NULL,
  INDEX `who_index`(`who`) USING BTREE,
  INDEX `time_index`(`time`) USING BTREE,
  INDEX `how_index`(`how`) USING BTREE,
  INDEX `vnum_index`(`vnum`) USING BTREE
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for loginlog
-- ----------------------------
DROP TABLE IF EXISTS `loginlog`;
CREATE TABLE `loginlog`  (
  `type` varbinary(20) NULL DEFAULT NULL COMMENT 'contains: LOGIN, LOGOUT',
  `time` datetime(0) NULL DEFAULT NULL,
  `channel` int(11) NULL DEFAULT NULL,
  `account_id` int(10) UNSIGNED NULL DEFAULT NULL,
  `pid` int(10) UNSIGNED NULL DEFAULT NULL,
  `level` int(11) NULL DEFAULT NULL,
  `job` int(11) NULL DEFAULT NULL,
  `playtime` int(10) UNSIGNED NULL DEFAULT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for loginlog2
-- ----------------------------
DROP TABLE IF EXISTS `loginlog2`;
CREATE TABLE `loginlog2`  (
  `type` varbinary(20) NULL DEFAULT NULL COMMENT 'contains: VALID, INVALID',
  `is_gm` varbinary(20) NULL DEFAULT NULL COMMENT 'contains: Y, N',
  `login_time` datetime(0) NULL DEFAULT NULL,
  `channel` int(11) NULL DEFAULT NULL,
  `account_id` int(10) UNSIGNED NULL DEFAULT NULL,
  `pid` int(10) UNSIGNED NULL DEFAULT NULL,
  `ip` int(10) UNSIGNED NULL DEFAULT NULL COMMENT 'inet_aton(\'%s\')',
  `client_version` varbinary(11) NULL DEFAULT NULL COMMENT 'maybe 1215955205',
  `logout_time` datetime(0) NULL DEFAULT NULL,
  `playtime` datetime(0) NULL DEFAULT NULL,
  `id` int(11) NULL DEFAULT NULL COMMENT 'maybe primary'
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for money_log
-- ----------------------------
DROP TABLE IF EXISTS `money_log`;
CREATE TABLE `money_log`  (
  `time` datetime(0) NULL DEFAULT NULL,
  `type` int(11) NULL DEFAULT NULL,
  `vnum` int(11) NULL DEFAULT NULL,
  `gold` bigint(20) UNSIGNED NULL DEFAULT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Fixed;

-- ----------------------------
-- Table structure for okay_event
-- ----------------------------
DROP TABLE IF EXISTS `okay_event`;
CREATE TABLE `okay_event`  (
  `pid` int(10) UNSIGNED NULL DEFAULT NULL,
  `name` varchar(30) CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT NULL,
  `points` bigint(20) NULL DEFAULT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for pcbang_loginlog
-- ----------------------------
DROP TABLE IF EXISTS `pcbang_loginlog`;
CREATE TABLE `pcbang_loginlog`  (
  `time` datetime(0) NULL DEFAULT NULL,
  `pcbang_id` int(10) UNSIGNED NULL DEFAULT NULL,
  `ip` varbinary(15) NULL DEFAULT NULL,
  `pid` int(10) UNSIGNED NULL DEFAULT NULL,
  `play_time` int(10) UNSIGNED NULL DEFAULT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for quest_reward_log
-- ----------------------------
DROP TABLE IF EXISTS `quest_reward_log`;
CREATE TABLE `quest_reward_log`  (
  `quest_name` varbinary(56) NULL DEFAULT NULL,
  `pid` int(10) UNSIGNED NULL DEFAULT NULL,
  `level` int(10) UNSIGNED NULL DEFAULT NULL,
  `type` int(11) NULL DEFAULT NULL COMMENT 'contains: EXP, ITEM',
  `how` int(10) UNSIGNED NULL DEFAULT NULL,
  `hint` int(10) UNSIGNED NULL DEFAULT NULL,
  `when` datetime(0) NULL DEFAULT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for refinelog
-- ----------------------------
DROP TABLE IF EXISTS `refinelog`;
CREATE TABLE `refinelog`  (
  `pid` int(10) UNSIGNED NULL DEFAULT NULL,
  `item_name` varbinary(56) NULL DEFAULT NULL,
  `item_id` int(10) UNSIGNED NULL DEFAULT NULL,
  `step` int(11) NULL DEFAULT NULL,
  `time` datetime(0) NULL DEFAULT NULL,
  `is_success` tinyint(1) NULL DEFAULT NULL,
  `setType` varchar(20) CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT NULL COMMENT 'contains: HYUNIRON, POWER, SCROLL'
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for shout_log
-- ----------------------------
DROP TABLE IF EXISTS `shout_log`;
CREATE TABLE `shout_log`  (
  `when` datetime(0) NULL DEFAULT NULL,
  `where` int(11) NULL DEFAULT NULL,
  `empire` int(11) NULL DEFAULT NULL,
  `message` varbinary(512) NULL DEFAULT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for vcard_log
-- ----------------------------
DROP TABLE IF EXISTS `vcard_log`;
CREATE TABLE `vcard_log`  (
  `vcard_id` int(10) UNSIGNED NULL DEFAULT NULL,
  `x` int(10) UNSIGNED NULL DEFAULT NULL,
  `y` int(10) UNSIGNED NULL DEFAULT NULL,
  `hostname` varbinary(56) NULL DEFAULT NULL,
  `giver_name` varbinary(16) NULL DEFAULT NULL,
  `giver_ip` varbinary(15) NULL DEFAULT NULL,
  `taker_name` varbinary(16) NULL DEFAULT NULL,
  `taker_ip` varbinary(15) NULL DEFAULT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

SET FOREIGN_KEY_CHECKS = 1;