function leaderBoid (agentID, position, strength, velocity, state, attributes, inbox, neighbours)
	
	--print("AGENT "..agentID.." uses brain simple")
	
	magnitude = math.sqrt( (velocity.x*velocity.x)+(velocity.y*velocity.y)+(velocity.z*velocity.z) )

	direction = {}

	if (magnitude==0)
	then
		direction.x = 1;
		direction.y = 0;
		direction.z = 0;
	else
		direction.x = velocity.x/magnitude
		direction.y = velocity.y/magnitude
		direction.z = velocity.z/magnitude
	end

	angle = (math.pi/300)

	direction.x = direction.x*math.cos(angle)-direction.z*math.sin(angle);
	direction.z = direction.x*math.sin(angle)+direction.z*math.cos(angle);
	
	force = {}

	--INDEX BY INT!!
	force[1] = direction.x*3
	force[2] = 0
	force[3] = direction.z*3

	messages = {}

	print(force[1].." "..force[2].." "..force[3]);

	return force, strength, state, messages
end
