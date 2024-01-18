quest free_biolog_magic begin
	state start begin
		when login or levelup with pc.get_level() ==105 begin
			send_letter("Recompensa:Magia Biologului")
		end
		when button or info begin
	     say_title("Resetare Bonus Biolog")
            say("Salutare.Ultima misiune a biologului(de nivel 105)")
            say("are mici probleme si singura solutie pentru a")
            say("rezolva aceasta problema este sa re-alegi bonusul")
            say("folosind acest obiect.Poti gasi")
            say("acest item si la Magazinul General pentru 5kkk")
			say("")
			pc.give_item2(164401, 1)
			set_state(__COMPLETE__)
		end
	end

	state __COMPLETE__ begin
	end
end