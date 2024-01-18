import mariadb as db
import spdlog as spd
import time

ACCOUNT_1, ACCOUNT_2, PLAYER_1, PLAYER_2 = range(4)
POST_FIX = 'sv1'

connections = {
    ACCOUNT_1: dict(user="root", host="localhost", database="account", password=""),
    ACCOUNT_2: dict(user="root", host="localhost", database="account2", password=""),

    PLAYER_1: dict(user="root", host="localhost", database="player", password=""),
    PLAYER_2: dict(user="root", host="localhost", database="player2", password=""),
}


class Fusion:
    def __init__(self, logger):
        self.__logger = logger

        self.__connections = []
        self.__accounts = {}
        self.__players = {}

    def __del__(self):
        for connection in self.__connections:
            if not connection:
                continue

            connection.close()

        self.__connections.clear()
        self.__accounts.clear()
        self.__players.clear()

    def run(self):
        if not self.__connect():
            return

        try:
            self.__merge()
        except Exception as e:
            self.__logger.error(str(e))

    def __connect(self):
        try:
            self.__connections = [db.connect(**connections.get(i)) for i in range(len(connections.keys()))]
        except Exception as e:
            self.__logger.error(str(e))
            return False
        return True

    def __get_rows(self, n, table_name):
        conn = self.__connections[n]
        if not conn:
            return None

        cursor = conn.cursor()
        if not cursor:
            return None

        cursor.execute(f"SELECT * FROM `{table_name}`;")
        rows = cursor.fetchall()

        return [list(row) for row in rows] if rows else None

    def __merge(self):
        self.__merge_account()
        self.__merge_safebox()

        self.__merge_player()
        self.__merge_player_index()
        self.__merge_affect()
        self.__merge_quest()

        self.__merge_offline_shops()
        self.__merge_offline_shop_auction_offers()
        self.__merge_offline_shop_offers()
        self.__merge_offline_shop_safebox_items()
        self.__merge_offline_shop_safebox_valutes()
        self.__merge_offline_shop_items()
        self.__merge_items()

    def __merge_account(self):
        src_rows = self.__get_rows(ACCOUNT_1, "account")
        if not src_rows:
            return

        dst_rows = self.__get_rows(ACCOUNT_2, "account")

        if dst_rows:
            for row in src_rows:
                src_username = row[1].lower()
                for j in dst_rows:
                    dst_username = j[1].lower()
                    if src_username == dst_username:
                        self.__logger.warn(f'found username duplication {row[1]}')
                        row[1] += POST_FIX

        conn = self.__connections[ACCOUNT_2]
        cursor = conn.cursor()

        for row in src_rows:
            old_id = row[0]
            query = 'INSERT INTO account VALUES('

            # values (?,?,?...)
            for i in range(len(row)):
                if i == 0:
                    # AUTO_INCREMENT
                    query += 'NULL,'
                else:
                    query += '?,'
            query = query[:-1] + ');'

            # remove the id
            row = row[1:]

            self.__logger.debug(query)
            # self.__logger.error(str(row))

            # execute the query
            cursor.execute(query, row)
            conn.commit()

            new_id = cursor.lastrowid
            if new_id is None:
                self.__logger.error(f'cannot insert new account_id {old_id}')

            self.__accounts[old_id] = new_id
            self.__logger.info(f'account_id from {old_id} to {new_id}')
            # time.sleep(0.3)

    def __merge_player(self):
        src_rows = self.__get_rows(PLAYER_1, "player")
        if not src_rows:
            return

        dst_rows = self.__get_rows(PLAYER_2, "player")

        if dst_rows:
            for row in src_rows:
                # update account_id
                old_aid = row[1]
                row[1] = self.__accounts.get(old_aid)
                self.__logger.info(f'[player] account_id from {old_aid} to {row[1]}')

                # check for player name duplication
                src_name = row[2].lower()
                for j in dst_rows:
                    dst_name = j[2].lower()
                    if src_name == dst_name:
                        old_name = row[2]
                        row[2] = row[2] + POST_FIX
                        self.__logger.warn(f'found player name duplication {old_name} to {row[2]}')

        conn = self.__connections[PLAYER_2]
        cursor = conn.cursor()

        for row in src_rows:
            old_id = row[0]
            query = 'INSERT IGNORE INTO `player` VALUES('

            # values (?,?,?...)
            for i in range(len(row)):
                if i == 0:
                    # AUTO_INCREMENT
                    query += 'NULL,'
                else:
                    query += '?,'
            query = query[:-1] + ');'

            # remove the id
            row = row[1:]

            self.__logger.debug(query)

            # execute the query
            cursor.execute(query, row)
            conn.commit()

            new_id = cursor.lastrowid

            if new_id is None:
                self.__logger.error(f'cannot insert new player {old_id}')

            self.__players[old_id] = new_id
            # self.__logger.info(f'player_id from {old_id} to {new_id}')
            # time.sleep(0.3)

    def __merge_player_index(self):
        src_rows = self.__get_rows(PLAYER_1, "player_index")
        if not src_rows:
            return

        new_rows = []
        for row in src_rows:
            # update account_id
            old_aid = row[0]
            new_aid = self.__accounts.get(old_aid, None)
            if not new_aid:
                self.__logger.error(f'[player_index] no account_id {old_aid}')
                continue

            row[0] = new_aid
            for i in range(2, len(row) - 1 - 1):
                old_player_id = row[i]
                if old_player_id == 0:
                    continue

                new_pid = self.__players.get(old_player_id)
                if not new_pid:
                    self.__logger.error(f'[player_index] no player_id {old_player_id}')
                    row[i] = 0
                    continue
                row[i] = new_pid
            new_rows.append(row)

        conn = self.__connections[PLAYER_2]
        cursor = conn.cursor()

        for row in new_rows:
            query = 'INSERT INTO `player_index` VALUES('
            # values (?,?,?...)
            for i in range(len(row)):
                query += '?,'
            query = query[:-1] + ');'

            self.__logger.debug(query)

            # execute the query
            cursor.execute(query, row)
            conn.commit()
            # time.sleep(0.3)

    def __merge_affect(self):
        rows = self.__get_rows(PLAYER_1, "affect")
        if not rows:
            return

        new_rows = []
        for row in rows:
            pid = row[0]
            if pid == 0:
                continue

            new_pid = self.__players.get(pid, None)
            if not new_pid:
                self.__logger.error(f'unknown affect by pid {pid}')
                continue

            row[0] = new_pid
            new_rows.append(row)

        conn = self.__connections[PLAYER_2]
        cursor = conn.cursor()

        for row in new_rows:
            query = 'INSERT INTO `affect` VALUES('
            # values (?,?,?...)
            for i in range(len(row)):
                query += '?,'
            query = query[:-1] + ');'

            self.__logger.debug(query)

            # execute the query
            cursor.execute(query, row)
            conn.commit()
            # time.sleep(0.3)

    def __merge_quest(self):
        rows = self.__get_rows(PLAYER_1, "quest")
        if not rows:
            return

        new_rows = []
        for row in rows:
            old_pid = row[0]
            if old_pid == 0:  # event flag
                continue

            new_pid = self.__players.get(old_pid, None)
            if not new_pid:
                self.__logger.error(f'unknown quest by pid {old_pid}')
                continue

            row[0] = new_pid
            new_rows.append(row)

        conn = self.__connections[PLAYER_2]
        cursor = conn.cursor()

        for row in new_rows:
            query = 'INSERT INTO `quest` VALUES('
            # values (?,?,?...)
            for i in range(len(row)):
                query += '?,'
            query = query[:-1] + ');'

            self.__logger.debug(query)

            # execute the query
            cursor.execute(query, row)
            conn.commit()
            # time.sleep(0.3)

    def __merge_offline_shops(self):
        rows = self.__get_rows(PLAYER_1, "offlineshop_shops")
        if not rows:
            return

        new_rows = []
        for row in rows:
            old_pid = row[0]
            new_pid = self.__players.get(old_pid, None)
            if not new_pid:
                self.__logger.error(f'unknown offshop by pid {old_pid}')
                continue

            row[0] = new_pid
            new_rows.append(row)

        conn = self.__connections[PLAYER_2]
        cursor = conn.cursor()

        for row in new_rows:
            query = 'INSERT INTO `offlineshop_shops` VALUES('
            # values (?,?,?...)
            for i in range(len(row)):
                query += '?,'
            query = query[:-1] + ');'

            self.__logger.debug(query)

            # execute the query
            cursor.execute(query, row)
            conn.commit()
            # time.sleep(0.3)

    def __merge_offline_shop_auctions(self):
        rows = self.__get_rows(PLAYER_1, "offlineshop_auctions")
        if not rows:
            return

        new_rows = []
        for row in rows:
            old_pid = row[0]
            new_pid = self.__players.get(old_pid, None)
            if not new_pid:
                self.__logger.error(f'unknown offshop by pid {old_pid}')
                continue

            row[0] = new_pid
            new_rows.append(row)

        conn = self.__connections[PLAYER_2]
        cursor = conn.cursor()

        for row in new_rows:
            query = 'INSERT INTO `offlineshop_auctions` VALUES('
            # values (?,?,?...)
            for i in range(len(row)):
                query += '?,'
            query = query[:-1] + ');'

            self.__logger.debug(query)

            # execute the query
            cursor.execute(query, row)
            conn.commit()
            # time.sleep(0.3)

    def __merge_offline_shop_auction_offers(self):
        rows = self.__get_rows(PLAYER_1, "offlineshop_auction_offers")
        if not rows:
            return

        new_rows = []
        for row in rows:
            old_pid = row[0]
            new_pid = self.__players.get(old_pid, None)
            if not new_pid:
                self.__logger.error(f'unknown offshop by pid {old_pid}')
                continue

            row[0] = new_pid
            new_rows.append(row)

        conn = self.__connections[PLAYER_2]
        cursor = conn.cursor()

        for row in new_rows:
            query = 'INSERT INTO `offlineshop_auction_offers` VALUES('
            # values (?,?,?...)
            for i in range(len(row)):
                query += '?,'
            query = query[:-1] + ');'

            self.__logger.debug(query)

            # execute the query
            cursor.execute(query, row)
            conn.commit()
            # time.sleep(0.3)

    def __merge_offline_shop_offers(self):
        rows = self.__get_rows(PLAYER_1, "offlineshop_offers")
        if not rows:
            return

        new_rows = []
        for row in rows:
            old_pid = row[1]
            new_pid = self.__players.get(old_pid, None)
            if not new_pid:
                self.__logger.error(f'unknown offshop by pid {old_pid}')
                continue

            row[1] = new_pid
            new_rows.append(row)

        conn = self.__connections[PLAYER_2]
        cursor = conn.cursor()

        for row in new_rows:
            query = 'INSERT INTO `offlineshop_offers` VALUES('
            # values (?,?,?...)
            for i in range(len(row)):
                if i == 0:
                    # AUTO_INCREMENT
                    query += 'NULL,'
                else:
                    query += '?,'
            query = query[:-1] + ');'

            # remove the id
            row = row[1:]

            self.__logger.debug(query)

            # execute the query
            cursor.execute(query, row)
            conn.commit()
            # time.sleep(0.3)

    def __merge_offline_shop_safebox_items(self):
        rows = self.__get_rows(PLAYER_1, "offlineshop_safebox_items")
        if not rows:
            return

        new_rows = []
        for row in rows:
            old_pid = row[1]
            new_pid = self.__players.get(old_pid, None)
            if not new_pid:
                self.__logger.error(f'unknown offshop by pid {old_pid}')
                continue

            row[1] = new_pid
            new_rows.append(row)

        conn = self.__connections[PLAYER_2]
        cursor = conn.cursor()

        for row in new_rows:
            query = 'INSERT INTO `offlineshop_safebox_items` VALUES('
            # values (?,?,?...)
            for i in range(len(row)):
                if i == 0:
                    # AUTO_INCREMENT
                    query += 'NULL,'
                else:
                    query += '?,'
            query = query[:-1] + ');'

            # remove the id
            row = row[1:]

            self.__logger.debug(query)

            # execute the query
            cursor.execute(query, row)
            conn.commit()
            # time.sleep(0.3)

    def __merge_offline_shop_safebox_valutes(self):
        rows = self.__get_rows(PLAYER_1, "offlineshop_safebox_valutes")
        if not rows:
            return

        new_rows = []
        for row in rows:
            old_pid = row[0]
            new_pid = self.__players.get(old_pid, None)
            if not new_pid:
                self.__logger.error(f'unknown offshop by pid {old_pid}')
                continue

            row[0] = new_pid
            new_rows.append(row)

        conn = self.__connections[PLAYER_2]
        cursor = conn.cursor()

        for row in new_rows:
            query = 'INSERT INTO `offlineshop_safebox_valutes` VALUES('
            # values (?,?,?...)
            for i in range(len(row)):
                query += '?,'
            query = query[:-1] + ');'

            self.__logger.debug(query)

            # execute the query
            cursor.execute(query, row)
            conn.commit()
            # time.sleep(0.3)

    def __merge_offline_shop_items(self):
        rows = self.__get_rows(PLAYER_1, "offlineshop_shop_items")
        if not rows:
            return

        new_rows = []
        for row in rows:
            old_pid = row[1]
            new_pid = self.__players.get(old_pid, None)
            if not new_pid:
                self.__logger.error(f'unknown offshop by pid {old_pid}')
                continue

            row[1] = new_pid
            new_rows.append(row)

        conn = self.__connections[PLAYER_2]
        cursor = conn.cursor()

        for row in new_rows:
            query = 'INSERT INTO `offlineshop_shop_items` VALUES('
            # values (?,?,?...)
            for i in range(len(row)):
                if i == 0:
                    # AUTO_INCREMENT
                    query += 'NULL,'
                else:
                    query += '?,'
            query = query[:-1] + ');'

            # remove the id
            row = row[1:]

            self.__logger.debug(query)

            # execute the query
            cursor.execute(query, row)
            conn.commit()
            # time.sleep(0.3)

    def __merge_items(self):
        rows = self.__get_rows(PLAYER_1, "item")
        if not rows:
            return

        new_rows = []
        for row in rows:
            old_pid = row[1]
            new_pid = self.__players.get(old_pid, None) if row[2] != 'SAFEBOX' else self.__accounts.get(old_pid, None)
            if not new_pid:
                if row[2] == 'SAFEBOX':
                    self.__logger.error(f'unknown safebox item by account_id {old_pid}')
                else:
                    self.__logger.error(f'unknown item by pid {old_pid}')
                continue

            row[1] = new_pid
            new_rows.append(row)

        conn = self.__connections[PLAYER_2]
        cursor = conn.cursor()

        for row in new_rows:
            query = 'INSERT INTO `item` VALUES('
            # values (?,?,?...)
            for i in range(len(row)):
                if i == 0:
                    # AUTO_INCREMENT
                    query += 'NULL,'
                else:
                    query += '?,'
            query = query[:-1] + ');'

            # remove the id
            row = row[1:]

            self.__logger.debug(query)

            # execute the query
            cursor.execute(query, row)
            conn.commit()
            # time.sleep(0.3)

    def __merge_safebox(self):
        rows = self.__get_rows(PLAYER_1, "safebox")
        if not rows:
            return

        for row in rows:
            # update account id
            old_aid = row[0]
            new_aid = self.__accounts.get(old_aid)
            row[0] = new_aid
            self.__logger.info(f'[safebox] account_id from {old_aid} to {new_aid}')

        conn = self.__connections[PLAYER_2]
        cursor = conn.cursor()

        # insert rows into destination
        for row in rows:
            query = 'INSERT INTO safebox VALUES('

            # values (?,?,?...)
            for i in range(len(row)):
                query += '?,'
            query = query[:-1] + ');'

            self.__logger.debug(query)

            # execute the query
            cursor.execute(query, row)
            conn.commit()
            # time.sleep(0.3)


if __name__ == '__main__':
    logger = spd.SinkLogger('fusion', [spd.stdout_sink_mt(), spd.basic_file_sink_mt('fusion.log')])
    logger.set_level(spd.LogLevel.INFO)
    logger.info("Start the fusion.")
    f = Fusion(logger)
    f.run()
    logger.info("End the fusion.")
