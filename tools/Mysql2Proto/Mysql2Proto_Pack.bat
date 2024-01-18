rmdir "data" 2> nul & mkdir "data/items/protos" & mkdir "data/monsters/protos"

@Mysql2Proto -pim
@pause
