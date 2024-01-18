quest autohunt_tickets begin
    state start begin
        when 99000.use or 99001.use or 99002.use begin
            local itemVnum = item.get_vnum()

            say_title(string.format("%s:", item_name(itemVnum)))
            say("")

            local lang = pc.get_language()

            if pc.get_map_index() >= 10000 then
                say(gameforge[lang].autohunt_tickets._1)
                return
            elseif not pc.can_warp() then
                say(gameforge[lang].autohunt_tickets._2)
                return
            end

            local daysAvail = item.get_value(0)
            local dayString = nil
            if daysAvail > 1 then
                dayString = gameforge[lang].autohunt_tickets._4
            else
                dayString = gameforge[lang].autohunt_tickets._3
            end

            say(gameforge[lang].autohunt_tickets._5)
            say(string.format("%d %s %s", daysAvail, dayString, gameforge[lang].autohunt_tickets._6))
            say("")
            say(gameforge[lang].autohunt_tickets._7)
            if select(gameforge[lang].common.yes, gameforge[lang].common.no) == 1 then
                say_title(string.format("%s:", item_name(itemVnum)))
                say("")

                if daysAvail <= 0 then
                    say(gameforge[lang].autohunt_tickets._8)
                    return
                end

                if not pc.can_warp() then
                    say(gameforge[lang].autohunt_tickets._2)
                    return
                end

                pc.remove_item(itemVnum, 1)
                local r = pc.give_premium_hunt(daysAvail)
                if r == 1 then
                    say(string.format("%s %d %s", gameforge[lang].autohunt_tickets._9, daysAvail, dayString))
                    say(gameforge[lang].autohunt_tickets._6)
                    wait()
                    pc.warp(pc.get_x() * 100, pc.get_y() * 100)
                    return
                else
                    pc.give_item(itemVnum, 1)
                    say(gameforge[lang].autohunt_tickets._10)
                end
            end
        end
    end
end
