DATE != date +%Y%m%d-%H%M%S

MY_HOST = localhost
MY_USER = root
MY_PASS = Ada4234Axz!643Qd

MY_LOGF = output.txt

createdb:
	# createdb:: print date start
	@echo "### createdb begin $(DATE)" >> $(MY_LOGF)

	# createdb:: account
	@echo "creating database account..." >> $(MY_LOGF)
	@echo "DROP DATABASE IF EXISTS "account";" | mysql -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) >> $(MY_LOGF)
	@echo "CREATE DATABASE "account";" | mysql -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) >> $(MY_LOGF)

	@echo "creating account tables..." >> $(MY_LOGF)
	@mysql -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) account < account.sql >> $(MY_LOGF)


	# createdb:: common
	@echo "creating database common..." >> $(MY_LOGF)
	@echo "DROP DATABASE IF EXISTS "common";" | mysql -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) >> $(MY_LOGF)
	@echo "CREATE DATABASE "common";" | mysql -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) >> $(MY_LOGF)

	@echo "creating common tables..." >> $(MY_LOGF)
	@mysql -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) common < common.sql >> $(MY_LOGF)


	# createdb:: log
	@echo "creating database log..." >> $(MY_LOGF)
	@echo "DROP DATABASE IF EXISTS "log";" | mysql -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) >> $(MY_LOGF)
	@echo "CREATE DATABASE "log";" | mysql -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) >> $(MY_LOGF)

	@echo "creating log tables..." >> $(MY_LOGF)
	@mysql -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) log < log.sql >> $(MY_LOGF)


	# createdb:: player
	@echo "creating database player..." >> $(MY_LOGF)
	@echo "DROP DATABASE IF EXISTS "player";" | mysql -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) >> $(MY_LOGF)
	@echo "CREATE DATABASE "player";" | mysql -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) >> $(MY_LOGF)

	@echo "creating player tables..." >> $(MY_LOGF)
	@mysql -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) player < player.sql >> $(MY_LOGF)

	@echo "### createdb end $(DATE)" >> $(MY_LOGF)
	# createdb:: print date end
