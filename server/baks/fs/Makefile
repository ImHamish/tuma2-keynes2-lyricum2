#### @martysama0134 backup scripts ####
## Inside /etc/crontab paste: (for automatic backups every hour)
# 0	*	*	*	*	root	make -C /home/metin2/baks/fs dump
## for automatic clean of backups older than 7 days every week:
# 0	0	*	*	1	root	make -C /home/metin2/baks/fs wclean

DATE != date +%Y%m%d-%H%M%S

CURPATH != pwd
PATH = ../../
FOLD = srv1
PATH2 = $(PATH)$(FOLD)
OUTPATH = .

all:
	@echo "available all, dump, recovery and clean"

dump:
	@cd $(PATH) && ./clear.py
	@cd $(CURPATH) && cd $(PATH) && tar -czf mt2_$(DATE).tgz $(FOLD)
	@mv $(PATH)mt2_$(DATE).tgz $(OUTPATH)
	@echo "backup di $(PATH2) finito"

recovery:
	@tar -xzf mt2.tgz -C $(PATH)
	@echo "recovery di $(PATH2) finito"

clean:
	@rm -f $(OUTPATH)/*.tgz

lclean:
	@cat /dev/null > $(MY_LOGF)

wclean:
	@echo "### clean week begin $(DATE)" >> $(MY_LOGF)
	@find $(OUTPATH) -name "*.tgz" -type f -mtime +7 -print -delete >> $(MY_LOGF)
	@echo "### clean week end $(DATE)" >> $(MY_LOGF)
