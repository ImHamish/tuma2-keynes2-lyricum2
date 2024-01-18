quest events_mgr begin
	state start begin
		-- christmas event
		when 20031.chat."Ho ho ho!" with game.get_event_flag("christmas_event") == 1 begin
			local lang = pc.get_language()
			
			say_title(string.format("%s:", mob_name(20031)))
			say("")
			say(string.format(gameforge[lang].events_mgr._19, pc.get_name()))
			local menu = select(gameforge[lang].events_mgr._20, gameforge[lang].events_mgr._21, gameforge[lang].events_mgr._22, gameforge[lang].common.close)
			if menu == 1 then
				npc.open_shop(71)
				setskin(NOWINDOW)
			elseif menu == 2 then
				command("cube open")
				setskin(NOWINDOW)
			elseif menu == 3 then
				cmdchat("BINARY_WHEEL_ASKOPEN")
				setskin(NOWINDOW)
			end
		end
		-- end christmas event

		-- ox event
		when 20011.chat."OX-Competition" begin
			local lang = pc.get_language()
			local stat = game.get_event_flag("oxevent_status")
			say_title(string.format("%s:", mob_name(20011))) 
			say("")
			say(gameforge[lang].events_mgr._2)
			say(gameforge[lang].events_mgr._3)
			say(gameforge[lang].events_mgr._4)
			say("")
			if stat == 0 then 
				say(gameforge[lang].events_mgr._5)
				say(gameforge[lang].events_mgr._6)
			elseif stat == 1 then 
				say(gameforge[lang].events_mgr._7)
				say(gameforge[lang].events_mgr._8)
				say("")
				local what = select(gameforge[lang].events_mgr._9, gameforge[lang].events_mgr._10, gameforge[lang].events_mgr._11) 
				if what == 1 then 
					say_title(string.format("%s:", mob_name(20011))) 
					say("")
					
					if pet.count_summoned() >= 1 or newpet.count_summoned() >= 1 then
						say(gameforge[lang].events_mgr._25)
						return
					end
					
					say(gameforge[lang].events_mgr._12)
					say("")
					wait()
					pc.warp(896500, 24600) 
				elseif what == 2 then 
					say_title(string.format("%s:", mob_name(20011))) 
					say("")
					say(gameforge[lang].events_mgr._13)
					say(gameforge[lang].events_mgr._14)
					say("")
					wait()
					pc.warp(896300, 28900) 
				end 

			elseif stat == 2 then
				say(gameforge[lang].events_mgr._15)
				say(gameforge[lang].events_mgr._16)
				say(gameforge[lang].events_mgr._17)
				local agree = select(gameforge[lang].common.yes, gameforge[lang].common.no) 
				if agree == 1 then 
					say_title(string.format("%s:", mob_name(20011))) 
					say("")
					say(gameforge[lang].events_mgr._18)
					say("")
					wait()
					pc.warp(896300, 28900)
				end
			end 
		end
		-- end ox event

		-- dungeon bonus
		when 5001.kill begin
			game.drop_item_with_ownership(89001, 1)
		end

		when 5002.kill begin
			game.drop_item_with_ownership(89002, 1)
		end

		when 5003.kill begin
			game.drop_item_with_ownership(89003, 1)
		end
		-- end dungeon bonus

		when 20023.chat."Event Manager" with pc.is_gm() and pc.get_name() == "[SA]AstaMicu" or pc.get_name() == "[SA]Tiganu" or pc.getname() == "[D]Nozarashi" or pc.getname() =="[DEV]IC3" begin
			say_title("Event Manager:")
			say("")
			say("Scegli un evento:")
			local events = select("Dungeon Boss Bonus", "Gara OX", "Evento Natale", "Chiudi")
			if events == 1 then
				local s = game.get_event_flag("dungeon_bonus")
				say_title("Event Manager:")
				say("")
				if s != 0 then
					say("L'evento al momento è attivo.")
					say("")
					say(string.format("Probabilità: +%d", s))
					say("")
					say("Cosa vuoi fare?")
					local what = select("Disattiva", "Cambia Probabilità ", "Chiudi")
					if what == 1 then
						say_title("Event Manager:")
						say("")
						say("Sei sicuro di voler disattivare l'evento?")
						local agree = select("Si", "No")
						if agree == 2 then
							return
						end
						
						say_title("Event Manager:")
						say("")
						say("L'evento è stato disattivato.")
						game.set_event_flag("dungeon_bonus", 0)
						notice_all(1295, "10")
					elseif what == 2 then
						say_title("Event Manager:")
						say("")
						say("Inserisci la nuova probabilità che il boss")
						say("può uscire quando si completa il dungeon.")
						say("min: 1")
						say("max: 90")
						local prob = input()
						say_title("Event Manager:")
						say("")
						if prob == nil or prob == "" then
							say("Il valore non è valido.")
							return
						end
						
						prob = tonumber(prob)
						if prob < 1 then
							say("La probabilità è minore di 1.")
						elseif prob > 90 then
							say("La probabilità è maggiore di 90.")
						end
						say(string.format("Hai inserito la probabilità: %d", prob))
						say("")
						say("Sei sicuro di voler procedere?")
						local agree = select("Si", "No")
						if agree == 2 then
							return
						end
						
						say_title("Event Manager:")
						say("")
						say(string.format("La probabilità è stata cambiata con %d.", prob))
						game.set_event_flag("dungeon_bonus", prob)
						notice_all(1295, string.format("%d", prob + 10))
					end
				else
					say("L'evento al momento è disattivo.")
					say("")
					say("Vuoi attivarlo?")
					say("")
					local agree = select("Si", "No")
					if agree == 2 then
						return
					end
					
					say_title("Event Manager:")
					say("")
					say("Inserisci la probabilità che il boss può ")
					say("uscire quando si completa il dungeon.")
					say("min: 1")
					say("max: 90")
					local prob = input()
					say_title("Event Manager:")
					say("")
					if prob == nil or prob == "" then
						say("Il valore non è valido.")
						return
					end
					
					prob = tonumber(prob)
					if prob < 1 then
						say("La probabilità è minore di 1.")
					elseif prob > 90 then
						say("La probabilità è maggiore di 90.")
					end
					say(string.format("Hai inserito la probabilità: %d", prob))
					say("")
					say("Sei sicuro di voler procedere?")
					local agree = select("Si", "No")
					if agree == 2 then
						return
					end
					
					say_title("Event Manager:")
					say("")
					say("L'evento è stato attivato.")
					game.set_event_flag("dungeon_bonus", prob)
					notice_all(1295, string.format("%d", prob + 10))
				end
			elseif events == 2 then
				say_title("Event Manager:")
				say("")
				if pc.get_map_index() != 113 then
					say("Devi essere nella mappa OX per poter")
					say("usare questa opzione.")
					say("")
					say("usa /warp 8963 289")
					return
				end
				
				local s = oxevent.get_status()
				if s == 0 then
					say("Cosa vuoi fare?")
					local what = select("Attiva", "Chiudi")
					if what == 1 then
						say_title("Event Manager:")
						say("")
						local res = oxevent.open()
						if res == 1 then
							say("Caricamento delle domande concluso con successo.") 
							say("Gli utenti possono ora partecipare all'evento.")
							notice_all(912, "") 
							notice_all(913, "") 
						else
							say("Caricamento delle domande fallito.")
							say("Controlla il file delle domande.") 
						end
					end
				elseif s == 1 then
					say(string.format("Numero partecipanti: %d.", oxevent.get_attender()))
					say("")
					say("Cosa vuoi fare?")
					local what = select("Blocca entrate", "Chiudi")
					if what == 1 then
						oxevent.close()
						say_title("Event Manager:")
						say("")
						say("Entrate per la gara OX bloccate.")
						notice_all(914, "")
						notice_all(915, "")
					end
				elseif s == 2 then
					say("Evento iniziato.")
					say("")
					say("Cosa vuoi fare?")
					local what = select("Poni domanda", "Distribuisci premio", "Termina evento", "Chiudi")
					if what == 1 then
						say_title("Event Manager:")
						say("")
						local res = oxevent.quiz(1, 30)
						if res == 1 then 
							say("Domande caricate con successo.")
						else
							say("Caricamento domande fallito.")
						end
					elseif what == 2 then
						say_title("Event Manager:")
						say("")
						say("Inserisci il vnum del premio da") 
						say("distribuire:") 
						local vnum = input()
						say_title("Event Manager:")
						say("")
						if vnum == nil or vnum == "" then
							say("Il valore non è valido.")
							return
						end
						
						vnum = tonumber(vnum)
						say("Inserire la quantità del premio") 
						say("da distribuire:")
						local count = input()
						say_title("Event Manager:")
						say("")
						if count == nil or count == "" then
							say("Il valore non è valido.")
							return
						end
						
						count = tonumber(count)
						say(string.format("Hai scelto: x%d %s", count, item_name(vnum)))
						say("")
						say("Sei sicuro?")
						local agree = select("Si", "No")
						if agree == 1 then
							oxevent.give_item(vnum, count)
							say_title("Event Manager:")
							say("")
							say(string.format("Hai consegnato i premi ai %d vincitori.", oxevent.get_attender()))
							notice_all(916, "")
						end
					elseif what == 3 then
						notice_all(917, "")
						oxevent.end_event()
						say_title("Event Manager:")
						say("")
						say("Gara OX terminata.")
					end
				else
					say("E' in corso una domanda, attendere...") 
				end
			elseif events == 3 then
				local s = game.get_event_flag("christmas_event")
				say_title("Event Manager:")
				say("")
				if s != 0 then
					say("L'evento al momento è attivo.")
					say("")
					say("Cosa vuoi fare?")
					local what = select("Disattiva", "Chiudi")
					if what == 1 then
						say_title("Event Manager:")
						say("")
						say("Sei sicuro di voler disattivare l'evento?")
						local agree = select("Si", "No")
						if agree == 2 then
							return
						end
						
						say_title("Event Manager:")
						say("")
						say("L'evento è stato disattivato.")
						game.set_event_flag("christmas_event", 0)
						notice_all(1304, "")
					end
				else
					say("L'evento al momento è disattivo.")
					say("")
					say("Vuoi attivarlo?")
					say("")
					local agree = select("Si", "No")
					if agree == 2 then
						return
					end
					
					say_title("Event Manager:")
					say("")
					say("L'evento è stato attivato.")
					game.set_event_flag("christmas_event", 1)
					notice_all(1303, "")
				end
			end
		end
	end
end

