quest free_biolog_magic2 begin
	state start begin
		when login or levelup with pc.get_level() ==110 begin
			send_letter("Recompensa:Magia Biologului 2")
		end
		when button or info begin
	     say_title("Resetare Bonus Biolog")
            say("Salutare.Ultima misiune a biologului(de nivel 110)")
            say("are aceasi problema ca si misiunea de 105") 
			say("si singura solutie pentru a")
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