function simple (agentID, position, strength, velocity, state, attributes, inbox, neighbours)
	
	--print("AGENT "..agentID.." uses brain simple")
	
	messages = {}
	index = 1;

	--for key,neighbour in pairs(neighbours)
	--do
	--	messages[index] = {}
	--	messages[index][1] = "hello"
	--	messages[index][2] = "bye"
	--	index = index + 1;
	--end
	
	--for key,m in pairs(inbox)
	--do
	--	print("processing message:")
	--	print(m.agentID,m.label,m.position,m.strength)
	--end

	force = {strength,0,0}

	--print("sending ",force,strength,messages,"\n")

	return force, strength, state, messages
end
