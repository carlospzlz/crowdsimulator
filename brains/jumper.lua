function jumper(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
	
	stateAction = {}
	--flyingHeight = 4

	function jumperOnGround(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
		
		--STRENGTH TO CHANGE HEIGHT OF JUMPING
		if (strength < 0)
		then
			strength = maxStrength
		else
			strength = strength - 0.1
		end

		-- JUMP
		jumpDirection = {0,0,0}

		XZw = 1
		Yw = 3
		jumpDirection[1] = (math.random()*2-1) * XZw
		jumpDirection[2] = Yw
		jumpDirection[3] = (math.random()*2-1) * XZw

		force = {}
		force[1] = jumpDirection[1] * strength
		force[2] = jumpDirection[2] * strength
		force[3] = jumpDirection[3] * strength
		
		return force, {0,0,0}, strength, "jumperFlying", {}
	end

	stateAction.jumperOnGround = jumperOnGround


	function jumperFlying(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
		
		-- CHECK IF ON GROUND
		if (position.y < 0.05)
		then
			return {0,0,0}, {0,0,0}, strength, "jumperOnGround", {}
		end

		return {0,0,0}, {0,0,0}, strength, state, {}
	end

	stateAction.jumperFlying = jumperFlying


	--FIRE TRANSITION	
	if not (stateAction[state])
	then
		print("Jumper behaviour: WARNING: unknown state "..state)
		return stateAction.jumperOnGround(agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)
	else
		return stateAction[state](agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)
	end

end
