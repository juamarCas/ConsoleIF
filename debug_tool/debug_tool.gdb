# this is a script to automate sone settings commands


set pagination off


define bpc
	br ConsoleIf.cpp:process_command
end

define pc
	p commands[$1]
end

define pcm
	p *current_map
end


define pcit
	p *command_it
end

define bpre
	br Result.h:set_error
end


define help
	print "---- help menu ----"
	print "bpc   -> break in process command function"
	print "pc    -> print command"
	print "pcm   -> print current map variable"
	print "pcit  -> print command iterator"
	print "bpre  -> set a break point in set error"
end
