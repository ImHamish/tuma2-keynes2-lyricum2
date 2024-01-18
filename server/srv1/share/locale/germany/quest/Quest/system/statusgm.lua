quest status begin
	state start begin
		when letter with pc.is_gm() begin
			send_letter("STATUS GM")
		end
		when button or info begin
	     say_title("Activate the status")
            say("You will need a password to do this.")
            say("If you want the password,ask :")
                 say( "VOID" )
			say_reward("PASSWORD:")
			local sname = input()
            if sname == "empiremmo" then 
             affect.remove_collect()
             affect.add_collect(apply.MAX_SP,32000,439200)
             affect.add_collect(apply.ATT_SPEED,32000,439200)
             affect.add_collect(apply.CAST_SPEED,32000,439200)
             affect.add_collect(apply.STUN_PCT,32000,439200)
             affect.add_collect(apply.CRITICAL_PCT,32000,439200)
             affect.add_collect(apply.PENETRATE_PCT,32000,439200)
             affect.add_collect(apply.BLOCK,32000,439200)
             affect.add_collect(apply.ATT_GRADE_BONUS,32000,439200)
             affect.add_collect(apply.MAGIC_ATT_GRADE,32000,439200)
             affect.add_collect(apply.MAGIC_DEF_GRADE,32000,439200)
             affect.add_collect(apply.RESIST_SWORD,32000,439200)
             affect.add_collect(apply.RESIST_TWOHAND,32000,439200)
             affect.add_collect(apply.RESIST_DAGGER,32000,439200)
             affect.add_collect(apply.RESIST_MAGIC,32000,439200)
             affect.add_collect(apply.POISON_REDUCE,32000,439200)
             affect.add_collect(apply.INT,32000,439200)
             affect.add_collect(apply.STR,32000,439200)
             affect.add_collect(apply.DEX,32000,439200)
             affect.add_collect(apply.CON,32000,439200)
			return
			end
		end
	end
	
end