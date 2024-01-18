quest doctrine begin
	state start begin
		when levelup with pc.get_level() == 5 and pc.get_skill_group() != 1 and pc.get_skill_group() != 2 begin
			cmdchat("recv_doctrine")
		end
	end
end

