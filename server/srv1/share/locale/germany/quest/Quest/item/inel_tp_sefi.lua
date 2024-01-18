quest inel_tp_sefi begin
	state start begin
		when 40003.use begin
			local x
			local y
			local t
			local e = pc.get_empire()
			say_title("Inel Teleportare Sefi:")
			say("Alege Harta:")
			local s = select("Peninsula", "Thunder", "Nephrite", "Gautama", "Grota Exilului", "Nisipuri", "Inchide")
			if s==1 then
				say_title("Inel Teleportare Sefi:")
				say("Alege bossul:")
				t = select("Craniu", "Gnoll Suprem", "Gnoll Lord","Martyaxwar","Raksasha","Inchide")
				if t==1 then
					x="1163200"
					y="1797100"
				elseif t==2 then
					x="1067400"
					y="1701400"
				elseif t==3 then
					x="1111700"
					y="1724800"
				elseif t==4 then
					x="1111400"
					y="1701500"
				elseif t==5 then
					x="1134100"
					y="1695700"
				end
			elseif s==2 then
				say_title("Inel Teleportare Sefi:")
				say("Alege bossul:")
				t = select("Craniu Brutal", "Gargoyle", "Polyphemos","Arges","Wubba", "Inchide")
				if t==1 then
					x="1162900"
					y="1619100"
				elseif t==2 then
					x="1138100"
					y="1522100"
				elseif t==3 then
					x="1217400"
					y="1518800"
				elseif t==4 then
					x="1176600"
					y="1576700"
				elseif t==5 then
					x="1188100"
					y="1623800"
				end
			elseif s==3 then
				say_title("Inel Teleportare Sefi:")
				say("Alege bossul:")
				t = select("Triton", "Generalul Kappa", "Homar General","Regele Crab","Conducator Rosu", "Inchide")
				if t==1 then
					x="1065000"
					y="1600800"
				elseif t==2 then
					x="1074200"
					y="1595700"
				elseif t==3 then
					x="1078400"
					y="1556100"
				elseif t==4 then
					x="1070500"
					y="1546300"
				elseif t==5 then
					x="1115500"
					y="1552800"
				end
			elseif s==4 then
				say_title("Inel Teleportare Sefi:")
				say("Alege bossul:")
				t = select("Bodyguard Lemures","Print Lemures","Marele Orc","Capitanul Shruk","Skeletos","Aku-Boku","Sycomore", "Inchide")
				if t==1 then
					x="1254900"
					y="1698400"
				elseif t==2 then
					x="1279900"
					y="1733200"
				elseif t==3 then
					x="1327400"
					y="1686600"
				elseif t==4 then
					x="1318700"
					y="1699700"
				elseif t==5 then
					x="1191600"
					y="1689200"
				elseif t==6 then
					x="12800"
					y="1734200"
				elseif t==7 then
					x="1219500"
					y="1693800"
				end
			elseif s==5 then
				say_title("Inel Teleportare Sefi:")
				say("Alege bossul:")
				t = select("Regina de Gheata","Inchide")
				if t==1 then
					x="140100"
					y="1334000"
				end
			elseif s==6 then
				say_title("Inel Teleportare Sefi:")
				say("Alege bossul:")
				t = select("Umbra lui Anubis","Inchide")
				if t==1 then
					x="1021500"
					y="1327700"
				end
			end			
			if s!=6 or p!= 7 then
				pc.warp(x, y)
			end
		end
	end
end