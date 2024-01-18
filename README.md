# tuma2-keynes2-lyricum2
 
## WARNING
THIS PROJECT ARE FOR EDUCATIONAL PURPOSES ONLY!

## Server: setup
pkg install git

pkg install python27

pkg install gmake

cd /

git clone https://github.com/yPincc/tuma2-keynes2-lyricum2

pkg install mariadb105-server-10.5.23

cp /tuma2-keynes2-lyricum2/setup/mariadb/server.cnf /usr/local/etc/mysql/conf.d

sysrc mysql_enable="YES"

service mysql-server start

mysql_secure_installation

mysql

CREATE USER 'mt2!user'@'localhost' IDENTIFIED BY '@mt2!pass';

GRANT ALL PRIVILEGES ON account.* TO 'mt2!user'@'localhost' IDENTIFIED BY '@mt2!pass';

GRANT ALL PRIVILEGES ON common.* TO 'mt2!user'@'localhost' IDENTIFIED BY '@mt2!pass';

GRANT ALL PRIVILEGES ON log.* TO 'mt2!user'@'localhost' IDENTIFIED BY '@mt2!pass';

GRANT ALL PRIVILEGES ON player.* TO 'mt2!user'@'localhost' IDENTIFIED BY '@mt2!pass';

CREATE USER 'proto'@'%' IDENTIFIED BY 'dev';

GRANT ALL PRIVILEGES ON player.item_proto TO 'proto'@'%' IDENTIFIED BY 'dev';

GRANT ALL PRIVILEGES ON player.mob_proto TO 'proto'@'%' IDENTIFIED BY 'dev';

FLUSH PRIVILEGES;

quit

cd /tuma2-keynes2-lyricum2/srcs

sh build_distribute.sh

cd /tuma2-keynes2-lyricum2/setup

gmake

cd /tuma2-keynes2-lyricum2/server

sh admin_panel.sh

## Client: setup
Download & Install Git & VS2022

Clone the project.

Run: srcs\create_vs_project.bat

Run: srcs\build_client_distribute.bat

Open: srcs\build\keynes2.sln

Set: Distribute

Compile: all the tools

Run: tools\FoxFSArchiver\create_all.bat

Copy: all the files from client-cfg folder to client folder
