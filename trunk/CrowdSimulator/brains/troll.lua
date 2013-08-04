function troll (agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
	
	--TROLL
	stateAction = {}

	function trollAttack(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)

		--CHECKING IF THERES IS CHANGE OF STATE
		enemiesCounter = 0
		for key, neighbour in pairs(neighbours)
		do
			if (neighbour.attributes.army~=attributes.army and
			    neighbour.state~="warriorDead" and
			    neighbour.state~="archerDead")
			then
				enemiesCounter = enemiesCounter + 1
			end
		end
		
		if (enemiesCounter>0)
		then
			return {0,0,0}, {0,0,0}, strength, "trollRun", {}
		end	

		--MOVEMENT
		closestEnemyPosition = {0,0,0}
		messages = {}
		distance = 99
		attackDistance = 5

		enemiesCounter = 1
		neighboursCounter = 1
		for key, neighbour in pairs(neighbours)
		do
			if (attributes.army~=neighbour.attributes.army)
			then
				tmpDistance = math.sqrt( (neighbour.position.x-position.x)^2 +
							 (neighbour.position.y-position.y)^2 +
							 (neighbour.position.z-position.z)^2 )
				if (tmpDistance<distance)
				then
					closestEnemyPosition[1]= neighbour.position.x
					closestEnemyPosition[2]= neighbour.position.y
					closestEnemyPosition[3]= neighbour.position.z

					distance = tmpDistance

					if (distance<attackDistance)
					then
						messages[neighboursCounter] = {}
						messages[neighboursCounter][0] = "superattack"
					end	
				end
				enemiesCounter = counter + 1
			end
			neighbours = neighboursCounter + 1
		end
		
		movementForce = {0,0,0}
		magnitude = 0
		heading = {0,0,0}
		if (enemiesCounter > 0)
		then
			movementForce[1] = (closestEnemyPosition[1] - position.x) - velocity.x
			movementForce[2] = (closestEnemyPosition[2] - position.y) - velocity.y
			movementForce[3] = (closestEnemyPosition[3] - position.z) - velocity.z

			magnitude = math.sqrt(movementForce[1]^2+movementForce[2]^2+movementForce[3]^2)
			heading[1] = movementForce[1] / magnitude
			heading[2] = movementForce[2] / magnitude
			heading[3] = movementForce[3] / magnitude
		end

		--INTEGRATE FORCES
		force = {}
		mw = 0.01
		--print(enemiesAttackForce[1],enemiesAttackForce[2],enemiesAttackForce[3])
		force[1] = movementForce[1]*mw
		force[2] = movementForce[2]*mw
		force[3] = movementForce[3]*mw

		return force, heading, strength, state, messages

	end
	
	stateAction.trollAttack = trollAttack

	-- TROLL RUN
	function trollRun(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
		
		--CHECK CHANGE OF STATE
		enemiesCounter = 0
		for key, neighbour in pairs(neighbours)
		do
			if (neighbour.attributes.army~=attributes.army and
			    neighbour.state~="warriorDead" and
			    neighbour.state~="archerDead")
			then
				enemiesCounter = enemiesCounter + 1
			end
		end
		
		if (enemiesCounter>0)
		then
			return {0,0,0}, {0,0,0}, strength, "trollAttack", {}
		end	
		
		
		--MOVEMENT
		angle = (math.pi/300)

		force = {0,0,0}
		force[1] = velocity.x*math.cos(angle)-velocity.z*math.sin(angle);
		force[3] = velocity.x*math.sin(angle)+velocity.z*math.cos(angle);

		magnitude = math.sqrt(force[1]^2 + force[2]^2 + force[3]^2)
		if (magnitude>0)
		then		
			force[1] = force[1] / magnitude
			force[2] = force[2] / magnitude
			force[3] = force[3] / magnitude
		else
			force[1] = 0
			force[2] = 0
			force[3] = 1
		end

		--FORCE
		fw = 1
		force[1] = force[1] * fw
		force[2] = force[2] * fw
		force[3] = force[3] * fw

		return force, {0,0,0}, strength, state, {}
	end

	stateAction.trollRun = trollRun
	
	
	--FIRE TRANSITION	
	if not (stateAction[state])
	then
		print("Troll behaviour: WARNING: unknown state "..state)
		return stateAction.trollHold(agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)
	else
		return stateAction[state](agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)
	end
end
