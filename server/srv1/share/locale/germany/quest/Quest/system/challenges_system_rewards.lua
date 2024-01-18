quest reward_check begin
	state start begin
		when levelup with pc.get_level() == 105 and not pc.is_gm() begin
			--REWARD_120=3
			game.set_reward(1)
			end
		when 2493.kill begin
			game.set_reward(6)
		end
		when 6091.kill begin
			game.set_reward(7)
		end
		when 6191.kill begin
			game.set_reward(8)
		end
	end
end
