# this is a script to automate sone settings commands


set pagination off


define pc
	br process_command
end

define pcm
	p *current_map
end


define pcit
	p *command_it
end


define help
	print "---- help menu ----\n"
	print "pc   -> break in process command function\n"
	print "pcm  -> print current map variable\n"
	print "pcit -> print command iterator\n"
end
