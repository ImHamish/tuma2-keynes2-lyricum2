SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for account
-- ----------------------------
DROP TABLE IF EXISTS `account`;
CREATE TABLE `account`  (
  `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `login` varchar(16) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL DEFAULT '' COMMENT 'LOGIN_MAX_LEN=30',
  `language` enum('EN','RO','IT','TR','DE','PL','PT','ES','CZ','HU') CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT 'EN',
  `password` varchar(42) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL DEFAULT '' COMMENT 'PASSWD_MAX_LEN=16; default 45 size',
  `social_id` varchar(7) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL DEFAULT '',
  `email` varchar(100) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL DEFAULT '',
  `status` varchar(8) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL DEFAULT 'OK',
  `availDt` datetime(0) NULL DEFAULT NULL,
  `banDescription` mediumtext CHARACTER SET latin1 COLLATE latin1_general_ci NULL,
  `create_time` datetime(0) NULL DEFAULT '2030-02-10 18:30:34',
  `last_play` datetime(0) NULL DEFAULT NULL,
  `gold_expire` datetime(0) NOT NULL DEFAULT '2000-00-00 00:00:00',
  `silver_expire` datetime(0) NOT NULL DEFAULT '2000-00-00 00:00:00',
  `safebox_expire` datetime(0) NOT NULL DEFAULT '2000-00-00 00:00:00',
  `autoloot_expire` datetime(0) NOT NULL DEFAULT '2000-00-00 00:00:00',
  `fish_mind_expire` datetime(0) NOT NULL DEFAULT '2000-00-00 00:00:00',
  `marriage_fast_expire` datetime(0) NOT NULL DEFAULT '2000-00-00 00:00:00',
  `money_drop_rate_expire` datetime(0) NOT NULL DEFAULT '2000-00-00 00:00:00',
  `bonus1_expire` datetime(0) NOT NULL DEFAULT '2000-00-00 00:00:00',
  `bonus2_expire` datetime(0) NOT NULL DEFAULT '2000-00-00 00:00:00',
  `bonus3_expire` datetime(0) NOT NULL DEFAULT '2000-00-00 00:00:00',
  `codice_conferma` varchar(40) CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT NULL,
  `real_name` varchar(16) CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT '',
  `question1` varchar(56) CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT NULL,
  `answer1` varchar(56) CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT NULL,
  `question2` varchar(56) CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT NULL,
  `answer2` varchar(56) CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT NULL,
  `coins` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `address` varchar(191) CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT NULL,
  `email_verified_at` datetime(0) NULL DEFAULT NULL,
  `ban_reason` varchar(191) CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT NULL,
  `referrer` varchar(191) CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT NULL,
  `gift_coins` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `session_key` varchar(16) CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT NULL,
  `hwid` varbinary(64) NULL DEFAULT '',
  `vote_time` datetime(0) NOT NULL DEFAULT current_timestamp,
  `web_admin` tinyint(4) NULL DEFAULT NULL,
  `jcoins` int(11) NOT NULL DEFAULT 0,
  `deletion_token` varchar(40) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL DEFAULT '',
  `passlost_token` varchar(40) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL DEFAULT '',
  `email_token` varchar(40) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL DEFAULT '',
  `new_email` varchar(64) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL DEFAULT '',
  `cash` varchar(64) CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT NULL,
  `v4c_last_date` datetime(0) NOT NULL DEFAULT '2022-08-28 20:04:18',
  `v4c_mp2s_net` tinyint(1) NOT NULL DEFAULT 0,
  `v4c_mp2s_info` tinyint(1) NOT NULL DEFAULT 0,
  `v4c_ip` varchar(191) CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT NULL,
  `v4c_votes_count` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE INDEX `login`(`login`) USING BTREE,
  INDEX `social_id`(`social_id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for antifarm
-- ----------------------------
DROP TABLE IF EXISTS `antifarm`;
CREATE TABLE `antifarm`  (
  `hwid` varbinary(64) NOT NULL,
  `login` varbinary(24) NOT NULL,
  `status` int(11) NOT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Fixed;

-- ----------------------------
-- Table structure for block_exception
-- ----------------------------
DROP TABLE IF EXISTS `block_exception`;
CREATE TABLE `block_exception`  (
  `login` varchar(16) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL DEFAULT ''
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for blocked_hwids
-- ----------------------------
DROP TABLE IF EXISTS `blocked_hwids`;
CREATE TABLE `blocked_hwids`  (
  `hwid` varbinary(64) NOT NULL DEFAULT '',
  `who` varbinary(64) NOT NULL DEFAULT '',
  `accounts` varbinary(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`hwid`) USING BTREE
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Fixed;

-- ----------------------------
-- Table structure for iptocountry
-- ----------------------------
DROP TABLE IF EXISTS `iptocountry`;
CREATE TABLE `iptocountry`  (
  `IP_FROM` varchar(16) CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT NULL,
  `IP_TO` varchar(16) CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT NULL,
  `COUNTRY_NAME` varchar(56) CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT NULL
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for string
-- ----------------------------
DROP TABLE IF EXISTS `string`;
CREATE TABLE `string`  (
  `name` varchar(64) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL DEFAULT '',
  `text` text CHARACTER SET latin1 COLLATE latin1_general_ci NULL DEFAULT NULL,
  PRIMARY KEY (`name`) USING BTREE
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for vote4bonus
-- ----------------------------
DROP TABLE IF EXISTS `vote4bonus`;
CREATE TABLE `vote4bonus`  (
  `ip` varbinary(128) NOT NULL DEFAULT '',
  `expire` datetime(0) NOT NULL DEFAULT current_timestamp,
  PRIMARY KEY (`ip`) USING BTREE
) ENGINE = MyISAM CHARACTER SET = latin1 COLLATE = latin1_general_ci ROW_FORMAT = Fixed;

SET FOREIGN_KEY_CHECKS = 1;