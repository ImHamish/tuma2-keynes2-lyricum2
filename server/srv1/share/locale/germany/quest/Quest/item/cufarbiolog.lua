quest baule_start begin
	state start begin
		when 60898.use begin
				pc.remove_item(60898, 1)
				pc.give_item2(40143,245)
				pc.give_item2(30006, 10)
				pc.give_item2(30047, 15)
				pc.give_item2(30015, 20)
				pc.give_item2(30050, 25)
				pc.give_item2(30165, 30)
				pc.give_item2(30166, 35)
				pc.give_item2(30167, 40)
				pc.give_item2(30168, 50)
				pc.give_item2(30251, 10)
				pc.give_item2(30252, 10)
		end
	end
end

