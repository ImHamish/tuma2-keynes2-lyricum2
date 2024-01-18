quest oua_truhen begin
        state start begin
            when login begin
                if game.get_event_flag("oua_event") == 1 then
                    notice("EVENT : EASTER EGGS - ACTIVE UNTIL 30.04.2022 !")
                end
            end
            when kill begin
                local m_ind = pc.get_map_index()
                if npc.is_pc() then
                else
                    if m_ind == 1 or m_ind == 3 or m_ind == 21 or m_ind == 23 or m_ind == 41 or m_ind == 43 and pc.get_level() <= 120 then
                        if game.get_event_flag("oua_event") == 1 then
                            local chance = number(1, 100)
                            local chance_max = game.get_event_flag("oua_drop_chance")
                            if chance <= chance_max then
								pc.give_item2(50162)
								pc.give_item2(50164)
								pc.give_item2(50166)
                                --game.drop_item(50161)
                            else
                            end
                        else
                        end
                    elseif m_ind == 61 or m_ind == 62 or m_ind == 63 or m_ind == 64 or m_ind == 65 or m_ind == 66 or m_ind == 67 or m_ind == 68 or m_ind == 69 and pc.get_level() <= 120 then
                        if game.get_event_flag("oua_event") == 1 then
                            local chance = number(1, 100)
                            local chance_max = game.get_event_flag("oua_drop_chance")
                            if chance <= chance_max then
								pc.give_item2(50162)
								pc.give_item2(50164)
								pc.give_item2(50166)
                                --game.drop_item(50161)
                            else
                            end
                        else
                        end
                    elseif m_ind == 70 or m_ind == 71 or m_ind == 72 or m_ind == 73 or m_ind == 103 or m_ind == 104 or m_ind == 105 and pc.get_level() <= 120 then
                        if game.get_event_flag("oua_event") == 1 then
                            local chance = number(1, 100)
                            local chance_max = game.get_event_flag("oua_drop_chance")
                            if chance <= chance_max then
								pc.give_item2(50162)
								pc.give_item2(50164)
								pc.give_item2(50166)
                                --game.drop_item(50161)
                            else
                            end
                        else
                        end
                    elseif m_ind == 110 or m_ind == 111 or m_ind == 208 or m_ind == 209 or m_ind == 210 or m_ind == 212 or m_ind == 216 or m_ind == 217 or m_ind == 218 and pc.get_level() <= 120 then
                        if game.get_event_flag("oua_event") == 1 then
                            local chance = number(1, 100)
                            local chance_max = game.get_event_flag("oua_drop_chance")
                            if chance <= chance_max then
								pc.give_item2(50162)
								pc.give_item2(50164)
								pc.give_item2(50166)
                                --game.drop_item(50161)
                            else
                            end
                        else
                        end
                    elseif m_ind == 301 or m_ind == 302 or m_ind == 303 or m_ind == 304 and pc.get_level() <= 120 then
                        if game.get_event_flag("oua_event") == 1 then
                            local chance = number(1, 100)
                            local chance_max = game.get_event_flag("oua_drop_chance")
                            if chance <= chance_max then
								pc.give_item2(50162)
								pc.give_item2(50164)
								pc.give_item2(50166)
                                --game.drop_item(50161)
                            else
                            end
                        else
                        end
                    elseif m_ind == 351 or m_ind == 352 or m_imd == 353 or m_ind == 354 or m_ind == 355 or m_imd == 356 or m_imd == 357 and pc.get_level() <= 120 then
                        if game.get_event_flag("oua_event") == 1 then
                            local chance = number(1, 100)
                            local chance_max = game.get_event_flag("oua_drop_chance")
                            if chance <= chance_max then
								pc.give_item2(50162)
								pc.give_item2(50164)
								pc.give_item2(50166)
                                --game.drop_item(50161)
                            else
                            end
                        else
                        end
                    end
                end
            end
            when 30129.chat."<GM> Eveniment Ouã de Paste" with pc.is_gm() begin
                if game.get_event_flag("oua_event") == 1 then
                    say_title("Administrare Eveniment:")
                    say("")
                    say("Ouã de Paste : ~ Activ ")
                    say("")
                    say_reward("Inchieire event ??")
                    local s=select("Da", "Nu")
                    if s==1 then
                        say_title("Administrare Eveniment:")
                        say("")
                        say("Evenimentul a luat sfârsit .")
                        say("~ Pe data viitoare !")
                        game.set_event_flag("oua_event", 0)
                        char_log(0, "oua_event_end from" ..pc.get_name())
                        notice_all("Evenimentul : Ouã de Paste ~ Terminat.")
                    elseif s==2 then
                        say_title("Administrare Eveniment:")
                        say("")
                        say("~ Pe curând !")
                    end
                else
                    say_title("Administrare Eveniment:")
                    say("")
                    say("Ouã de Paste : ~ Închis ")
                    say("")
                    say_reward("Start Eveniment?")
                    local s=select("Da", "Nu")
                    if s==1 then
                        say_title("Administrare Eveniment:")
                        say("")
                        say("Evenimentul ~ A Început ! .")
                        say("~ Succes la strans mai multe Ouã de Paste !")
                        game.set_event_flag("oua_event", 1)
                        char_log(0, "oua_event_start from" ..pc.get_name())
                        notice_all("Evenimentul : Ouã de Paste ~ A Început !")
                    elseif s==2 then
                        say_title("Administrare Eveniment:")
                        say("")
                        say("~ Pe curând !")
                    end
                end
            end
            when 30129.chat."<GM> Drop Ouã de Paste" with pc.is_gm() begin
                if pc.get_name() == "[SA]Moralised" or pc.get_name() == "[GA]salvE" or pc.get_name() == "[SA]Osaka" or pc.get_name() == "[SA]Pain" or pc.get_name() == "Moralised" then
                    say_title("Administrare Eveniment:")
                    say("")
                    say("Cat de mult sã fie dropul acestora ?")
                    say("")
                    say_reward("Între 1-100")
                    say("")
                    local new_chance = tonumber(input())
                    if new_chance < 0 or new_chance > 100 then
                        say_title("Administrare Eveniment:")
                        say("")
                        say("Introducere incorectã ~")
                    else
                        char_log(0, "oua_event_chance from" ..pc.get_name().. " to " ..new_chance.. "%")
                        game.set_event_flag("oua_drop_chance", new_chance)
                        say_title("Administrare Eveniment:")
                        say("")
                        say("Dropul actual " ..new_chance.. " setat.")
                        say("~ Pe curând !")
                    end
                else
                    say_title("Administrare Eveniment:")
                    say("")
                    say("Nu ai dreptul pentru a executa evenimentul .")
                end
            end
        end
    end 