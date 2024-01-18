ho "### Genero il locale_list... ###"
ls -R Quest/*/* > quest_list
sleep 2
echo "### Attualizzo le Quest... ###"
sleep 1
/usr/local/bin/python2.7 pre_qc.py -ac
sleep 1
echo "### Quest attualizate con sucesso! ###"
sleep 1
# cd / && clear
cd ..