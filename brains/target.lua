function target (agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)

	stateAction = {}

	function targetDamage(inbox)

		damage = 0;
		for key,message in pairs(inbox)
		do
			if (message.label=="shot")
			then
				damage = damage + message.strength
			end
		end

		return damage
	end

	--STRENGTH
	function targetMove (agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
	
		if (strength<0)
		then
			return {0,0,0},{0,0,0},0,"targetDead",{}
		end
		
		dw = 0.0001
		strength = strength - targetDamage(inbox)*dw

		speed = math.sqrt(velocity.x*velocity.x+velocity.y*velocity.y+velocity.z*velocity.z)

		direction = {}
		if (speed==0)
		then
			direction.x = math.cos(agentID*math.pi/4)
			direction.y = 0
			direction.z = math.sin(agentID*math.pi/4)
		else
			direction.x = velocity.x/speed
			direction.y = velocity.y/speed
			direction.z = velocity.z/speed
		end

		angle = (math.pi/400)

		direction.x = direction.x*math.cos(angle)-direction.z*math.sin(angle);
		direction.z = direction.x*math.sin(angle)+direction.z*math.cos(angle);

		--FORCE
		force = {}
		tw = 0.5
		force[1] = direction.x*strength*tw
		force[2] = 0
		force[3] = direction.z*strength*tw

		return force, {0,0,0}, strength, "targetMove", {}
	end
	
	stateAction.targetMove = targetMove

	function targetDead (agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
		
		return {0,0,0}, {0,0,0}, 0, "targetDead", {}
	end

	stateAction.targetDead = targetDead

	--FIRE TRANSITION	
	if not (stateAction[state])
	then
		print("Target behaviour: WARNING: unknown state "..state)
		return stateAction.targetMove(agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)
	else
		return stateAction[state](agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)
	end

end
