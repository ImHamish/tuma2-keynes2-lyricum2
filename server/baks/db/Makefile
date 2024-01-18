#### @martysama0134 backup scripts ####
### Inside /etc/crontab paste:
## for automatic backups every hour:
# 0	*	*	*	*	root	make -C /home/metin2/baks/db dump
## for automatic clean of backups older than 7 days every week:
# 0	0	*	*	1	root	make -C /home/metin2/baks/db wclean

DATE != date +%Y%m%d-%H%M%S

MY_HOST = localhost
MY_USER = bkuser
MY_PASS = ADkjSFRKAHJQ
MY_LOGF = log.txt
OUTPATH = .

all:
	@echo "available all, dump, recovery, clean, lclean, wclean, cleanall"
	@echo "TEST: -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) > $(DATE).sql"

dump:
	@echo "### dump begin $(DATE)" >> $(MY_LOGF)
	@/usr/local/bin/mysqldump --single-transaction --quick --lock-tables=false -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) account | gzip -9 > $(OUTPATH)/account_$(DATE).sql.gz 2>> $(MY_LOGF)
	@/usr/local/bin/mysqldump --single-transaction --quick --lock-tables=false -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) common | gzip -9 > $(OUTPATH)/common_$(DATE).sql.gz 2>> $(MY_LOGF)
	@/usr/local/bin/mysqldump --single-transaction --quick --lock-tables=false -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) player | gzip -9 > $(OUTPATH)/player_$(DATE).sql.gz 2>> $(MY_LOGF)
	@/usr/local/bin/mysqldump --single-transaction --quick --lock-tables=false --no-data -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) log | gzip -9 > $(OUTPATH)/log_$(DATE).sql.gz 2>> $(MY_LOGF)
#log-with-data	@/usr/local/bin/mysqldump --single-transaction --quick --lock-tables=false -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) log | gzip -9 > $(OUTPATH)/log_$(DATE).sql.gz 2>> $(MY_LOGF)
	@echo "### dump end $(DATE)" >> $(MY_LOGF)

recovery:
	@echo "### recovery begin $(DATE)" >> $(MY_LOGF)
	@gunzip < account.sql.gz | mysql -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) account 2>> $(MY_LOGF)
	@gunzip < common.sql.gz | mysql -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) common 2>> $(MY_LOGF)
	@gunzip < player.sql.gz | mysql -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) player 2>> $(MY_LOGF)
	@gunzip < log.sql.gz | mysql -u $(MY_USER) -p$(MY_PASS) -h $(MY_HOST) log 2>> $(MY_LOGF)
	@echo "### recovery end $(DATE)" >> $(MY_LOGF)

clean:
	@rm -f $(OUTPATH)/*.gz

lclean:
	@cat /dev/null > $(MY_LOGF)

wclean:
	@echo "### clean week begin $(DATE)" >> $(MY_LOGF)
	@find $(OUTPATH) -name "*.gz" -type f -mtime +7 -print -delete >> $(MY_LOGF)
	@echo "### clean week end $(DATE)" >> $(MY_LOGF)

cleanall: clean lclean
	@echo "### clean all done $(DATE)" >> $(MY_LOGF)



