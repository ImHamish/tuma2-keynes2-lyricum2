quest dungeons_ranking begin
	state start begin
		when 20421.chat."Ranking" begin
			say_title(mob_name(20420))
			say("")
			local lang = pc.get_language()
			local menu = select(gameforge[lang].dungeons_ranking._1, gameforge[lang].common.close)
			if menu == 2 then
				return
			end
			
			MeleyLair.OpenRanking()
			setskin(NOWINDOW)
		end
    end
end

