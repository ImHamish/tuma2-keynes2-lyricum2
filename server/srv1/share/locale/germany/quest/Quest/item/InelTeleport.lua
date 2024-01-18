quest teleportRing begin
	state start begin
		when 30897.use begin		
			TeleportRing.MainWindow(item.get_vnum());
		end
	end
end