#!/usr/bin/env python
# -*- coding: windows-1252 -*-

def run():
	source_filename = raw_input("Insert the source filename: ")
	try:
		f = open(source_filename)
		f.close()
	except Exception as e:
		print(str(e))
		input("Press enter to exit.")
	
	target_filename = raw_input("Insert the target filename: ")
	try:
		f = open(target_filename)
		f.close()
	except Exception as e:
		print(str(e))
		input("Press enter to exit.")
	
	result_filename = raw_input("Insert the result filename: ")
	
	source_list = []
	with open(source_filename) as f:
		for l in f:
			source_list.append(l)
	
	target_list = []
	with open(target_filename) as f:
		for l in f:
			target_list.append(l)
	
	save_list = []
	notfound_list = []
	
	for source_line in source_list:
		source_def_name = source_line.split("\t")
		act_len = 0
		max_len = len(target_list)
		
		for target_line in target_list:
			act_len += 1
			
			target_def_name = target_line.split("\t")
			if source_def_name[0] == target_def_name[0]:
				isHere = 1
				
				try:
					line = source_def_name[0] + "\t" + target_def_name[1] + "\t" + target_def_name[2] + "\t" + target_def_name[3] + "\t" + target_def_name[4]
				except:
					try:
						line = source_def_name[0] + "\t" + target_def_name[1] + "\t" + target_def_name[2] + "\t" + target_def_name[3]
					except:
						try:
							line = source_def_name[0] + "\t" + target_def_name[1] + "\t" + target_def_name[2]
						except:
							try:
								line = source_def_name[0] + "\t" + target_def_name[1]
							except:
								isHere = 0
								pass
				
				if isHere == 1:
					save_list.append(line)
				else:
					notfound_list.append(source_line)
				
				break
			
			if act_len == max_len:
				save_list.append(source_line)
				notfound_list.append(source_line)
	
	saved_filename = result_filename
	with open(saved_filename, "w") as f:
		for i in xrange(len(save_list)):
			f.write(save_list[i])
		
		f.close()
	
	notfound_filename = "notfound_" + result_filename
	with open(notfound_filename, "w") as f:
		for i in xrange(len(notfound_list)):
			f.write(notfound_list[i])
		
		f.close()

run()
