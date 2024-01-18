quest baule_start begin
	state start begin
		when 60899.use begin
				pc.remove_item(60899, 1)
				pc.give_item2(115400, 1)
				pc.give_item2(125400, 1)
				pc.give_item2(135400, 1)
				pc.give_item2(145400, 1)
				pc.give_item2(155400, 1)
				pc.give_item2(165400, 1)
				pc.give_item2(86062, 25)
				pc.give_item2(100100, 12)
				pc.give_item2(100500, 30)
		end
	end
end

