quest md_vouchers begin
	state start begin
		when 89001.use begin
			if not pc.can_warp() or pc.is_gm() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			pc.remove_item(89001, 1)
			pc.charge_cash(10)
		end

		when 89002.use begin
			if not pc.can_warp() or pc.is_gm() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			pc.remove_item(89002, 1)
			pc.charge_cash(20)
		end

		when 89003.use begin
			if not pc.can_warp() or pc.is_gm() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			pc.remove_item(89003, 1)
			pc.charge_cash(30)
		end

		when 89004.use begin
			if not pc.can_warp() or pc.is_gm() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			pc.remove_item(89004, 1)
			pc.charge_cash(50)
		end

		when 89005.use begin
			if not pc.can_warp() or pc.is_gm() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			pc.remove_item(89005, 1)
			pc.charge_cash(100)
		end

		when 89006.use begin
			if not pc.can_warp() or pc.is_gm() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			pc.remove_item(89006, 1)
			pc.charge_cash(500)
		end

		when 89007.use begin
			if not pc.can_warp() or pc.is_gm() then
				syschat(gameforge[pc.get_language()].reset_scroll._35)
				return
			end
			
			pc.remove_item(89007, 1)
			pc.charge_cash(1000)
		end
	end
end

