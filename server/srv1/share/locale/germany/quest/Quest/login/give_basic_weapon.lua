quest give_basic_weapon begin
    state start begin
        when login begin
            local myEmpire = pc.get_empire()

            horse.set_level(21)

            pc.set_skill_level(131, 10)
            pc.set_skill_level(137, 20)
            pc.set_skill_level(138, 20)
            pc.set_skill_level(139, 20)
            pc.set_skill_level(140, 20)

            if myEmpire == 1 then
                pc.set_skill_level(127, 40)
                pc.set_skill_level(128, 40)
            elseif myEmpire == 2 then
                pc.set_skill_level(126, 40)
                pc.set_skill_level(128, 40)
            elseif myEmpire == 3 then
                pc.set_skill_level(126, 40)
                pc.set_skill_level(127, 40)
            end

            local setmod = {
                { "block_exchange",          1,  "Trade" },
                { "block_party_invite",      2,  "Party" },
                { "block_guild_invite",      4,  "Guild" },
                { "block_whisper",           8,  "Whisper" },
                { "block_messenger_invite",  16, "Friend" },
                { "block_party_request",     32, "Request" }
            }

            local retnum = 0
            local retstr = ""
            table.foreachi(setmod,
                function(num, str)
                    if(pc.getf("game_option",setmod[num][1])==1)then
                        retnum=(retnum+setmod[num][2])
                        local delim=""
                        if(retstr~="")then delim="," end
                        retstr=(retstr..delim.." "..setmod[num][3])
                    end
                end
            )

            cmdchat("recv_doctrine")
            command("setblockmode "..retnum)

            local lang = pc.get_language()
            notice_all(904, pc.get_name())
            set_state(__COMPLETE__)
        end
    end

    state __COMPLETE__ begin
        when login begin
            local setmod = {
                { "block_exchange",          1,  "Trade" },
                { "block_party_invite",      2,  "Party" },
                { "block_guild_invite",      4,  "Guild" },
                { "block_whisper",           8,  "Whisper" },
                { "block_messenger_invite",  16, "Friend" },
                { "block_party_request",     32, "Request" }
            }

            local retnum = 0
            local retstr = ""
            table.foreachi(setmod,
                function(num, str)
                    if(pc.getf("game_option",setmod[num][1])==1)then
                        retnum=(retnum+setmod[num][2])
                        local delim=""
                        if(retstr~="")then delim="," end
                        retstr=(retstr..delim.." "..setmod[num][3])
                    end
                end
            )

            command("setblockmode "..retnum)
        end
    end
end
