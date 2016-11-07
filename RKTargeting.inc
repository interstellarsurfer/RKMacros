
Sub GetTarget
	/if (${DebugMode}) /echo GetTarget
	/declare i int local
	
	/if (${BOTMode} <= 2) /return FALSE
	
	/if ((!${Bool[${Group.Leader.ID}]} || ${IsTank} || (${Group.Leader.ID} == ${Me.ID}) && ${PullMode})) {
		/call GetMainTankTargetID
		/if (${DebugMode}) /echo main tank target = ${Macro.Return}
		/if (!${Macro.Return}) /return FALSE
		/varset CurrentTargetId ${Macro.Return}
	} else {
		/if (${Me.XTarget} == 0 || (${Me.XTarget} == 1 && !${Me.XTarget[1].Type.Equal[NPC]})) /return FALSE
		/call GetMainAssistTargetID
		/if (${DebugMode}) /echo main assist target = ${Macro.Return}		
		/varset CurrentTargetId ${Macro.Return}
	}
	
	/if (${DebugMode}) /echo GetTarget for ${Spawn[${CurrentTargetId}].CleanName}
	/if (${CurrentTargetId} != 0) /call SelectNewTarget ${CurrentTargetId}
/return ${Macro.Return}


Sub SelectNewTarget(int targetId)
	/if (${DebugMode}) /echo SelectNewTarget ${targetId}
	/if (!${targetId}) /return FALSE
	/if (${targetId} == 0) /return FALSE
	/if (${Target.ID} == ${targetId}) /return ${Target.ID}
	
	/if (${DebugMode}) /echo validating 2
	/call ValidateTarget ${targetId}
	/if (${DebugMode}) /echo Evaluation was ${Macro.Return}
	/if (TRUE) {
		/attack off
		/call TargetSync ${targetId}
		
		/if (${CurrentTargetId} != ${targetId}) {
			/varset CurrentTargetId ${targetId}
			/if (${DebugMode}) /echo SelectNewTarget - set CurrentTargetId to ${targetId} - ${CurrentTargetId}
		}
		
		/if (${Me.LAHoTT} > 0) {
			/if (${TankId} && !${IsTank} && !${Spawn[id ${TankId}].Class.Name.Equal[Magician]} && !${Me.Class.Name.Equal[Magician]}) {
				/if (${Me.TargetOfTarget.ID} && ${Me.TargetOfTarget.ID} != ${TankId}) /return FALSE
			}
		}
		
		/if (${Me.Pet.ID} && ${Me.Pet.Following.ID} != ${targetId}) {		
			/if (${DebugMode}) /echo backing pet off
			/pet back off
			/pet attack
		}
		
		/return ${targetId}
	}
/return FALSE

Sub GetMainTankTargetID
	|/if (${DebugMode}) 
	/if (${DebugMode}) /echo GetMainTankTargetID
	/if (!${Me.XTarget[1].ID} || !${KeepAgroOnAdds}) {
		/call GetMainAssistTargetID
		/return ${Macro.Return}
	}
	
	/declare xTargetId int local
	/declare i int local	
	/if (${Me.Class.Name.Equal[Magician]}) {
		/call GetPetMainTankID
		/if (${Macro.Return}) /return ${Macro.Return}
	}
	
	/call AreTargetsFacingMe
	/if (!${Macro.Return}) {
		/for i 1 to ${Me.XTarget}
			/if (${DebugMode}) /echo validating 5
			/varset xTargetId ${Me.XTarget[${i}].ID}		
			/call IsTargetFacingMe ${xTargetId}
			/if (!${Macro.Return}) {
				/call ValidateTarget ${xTargetId}
				/if (${Macro.Return}) {
					/xtar ${i}
					/delay 3
						
					/if (${Me.PctAggro} != 100) {
						/if (${DebugMode}) /echo I need to get agro on ${Target}
						/call SelectNewTarget ${xTargetId}
						/return ${xTargetId}
					}
				}
			}
		/next i
	} else {
		/call GetMainAssistTargetID
		/return ${Macro.Return}
	}
/return FALSE

Sub GetPetMainTankID
	/call AreTargetsFacingPet
	/if (!${Macro.Return}) {
		/for i 1 to ${Me.XTarget}
			/varset xTargetId ${Me.XTarget[${i}].ID}		
			/if (${DebugMode}) /echo validating 3
			/call IsTargetFacingPet ${xTargetId}
			/if (!${Macro.Return}) {
				/call ValidateTarget ${xTargetId}
				/if (${Macro.Return}) {
					/xtar ${i}
					/delay 2
						
					/if (${Me.PctAggro} != 100 && ${Bool[${Target.Mezzed}]}) {
						/if (${DebugMode}) /echo I need to get agro on ${Target}
						/call SelectNewTarget ${xTargetId}
						/return ${xTargetId}
					}
				}
			}
		/next i
	} else {
		/call GetMainAssistTargetID
		/return ${Macro.Return}
	}
/return FALSE

Sub GetMainAssistTargetID
	/if (${DebugMode}) /echo GetMainAssistTargetID
	/if (${Bool[${Me.XTarget[1].ID}]}) {
		/if (!${IsTank} && ${Me.GroupMarkNPC[1].ID} && ${Me.GroupMarkNPC[1].Type.Equal[NPC]}) /return ${Me.GroupMarkNPC[1].ID}
	}

	/if (${DebugMode}) /echo Getting main assist id
	
	/declare i int local
	/declare spawnIndex int local
	
	/if (!${Bool[${Me.XTarget[1].ID}]}) {			
		/if (${DebugMode}) /echo out of combat pull

		/if (!${PullMode}) {
			/if (${DebugMode}) /echo early pull escape
			/return FALSE
		}
		
		/if (${DebugMode}) /echo looking at priority targets
		/for i 1 to ${PriorityTargets.Size}
			/for spawnIndex 1 to 3
				/if (${Bool[${PriorityTargets[${i}]}]}) {
					/if (${DebugMode}) /echo evaluating PriorityTarget - ${PriorityTargets[${i}]}
					/varset PullTargetEvaluationId ${NearestSpawn[${spawnIndex}, NPC radius 5000 ${PriorityTargets[${i}]}].ID}
					/if (${PullTargetEvaluationId}) {
						/call ValidateTarget ${PullTargetEvaluationId}
						/if (${Macro.Return}) {
							/if (${DebugMode}) /echo Found PriorityTarget - ${PriorityTargets[${i}]}
							/varset PullTargetDistance ${Spawn[id ${PullTargetEvaluationId}].Distance}
							/varset PullTargetId ${PullTargetEvaluationId}
							/return ${PullTargetId}
						}
					}
				}
			/next spawnIndex
		/next i

		/if (${DebugMode}) /echo looking at LOS targets		
		/call GetLOSPullTargetId
		/if (${Macro.Return}) {
			/if (${PullTargetId} != 0 && ${Spawn[id ${PullTargetId}].Distance} < 5000) {
				|/if (${DebugMode}) 
				/if (${DebugMode}) /echo returning targetId ${PullTargetId}
				/varset PullTargetDistance ${Spawn[id ${PullTargetId}].Distance}
				/varset PullTargetId ${PullTargetId}
				/return ${PullTargetId}
			}
		}
			
		/if (${DebugMode}) /echo looking at pull targets			
		/for i 1 to ${PullTargets.Size}
			/if (${Bool[${PullTargets[${i}]}]}) {
				/for spawnIndex 1 to 3
					/if (${DebugMode}) /echo evaluating pull target ${PullTargets[${i}]}
					/varset PullTargetEvaluationId ${NearestSpawn[${spawnIndex}, NPC radius ${PullTargetDistance} ${PullTargets[${i}]}].ID}
					/call ValidateTarget ${PullTargetEvaluationId}
					/if (${Macro.Return}) {
						/if (${PullTargetEvaluationId} && ${Spawn[id ${PullTargetEvaluationId}].Distance} < ${PullTargetDistance}) {
							/if (${DebugMode}) /echo setting PullTargetEvaluationId to ${PullTargetEvaluationId} at distance ${Spawn[id ${PullTargetEvaluationId}].Distance}
							/varset PullTargetDistance ${Spawn[id ${PullTargetEvaluationId}].Distance}
							/varset PullTargetId ${PullTargetEvaluationId}
						}
					}
				/next spawnIndex
			}
		/next i
					
		/if (${DebugMode}) /echo GetMainAssistTargetID - Got target ${PullTargetId}
		/call SelectNewTarget ${PullTargetId}

		/return ${PullTargetId}
	}
	
	/declare lowestHPPct int local 101
	/declare lowestHPID int local 0
	/declare xTargetId int local
	
	/for i 1 to ${Me.XTarget}
		/varset xTargetId ${Me.XTarget[${i}].ID}
		/if (${Spawn[id ${xTargetId}].Type.Equal[NPC]}) {
			/if (${Spawn[id ${xTargetId}].PctHPs} < ${lowestHPPct}) {
				/if (${DebugMode}) /echo validating 1
				|/call ValidateTarget ${xTargetId}
				|/if (${Macro.Return}) {
					/varset lowestHPPct ${Spawn[id ${xTargetId}].PctHPs}
					/varset lowestHPID ${xTargetId}
				|}
			}
		}
	/next i
	
	/if (${lowestHPID}) {
		/call SelectNewTarget ${lowestHPID}
		/if (${Macro.Return}) {
			/call AddToIntArray "BadTargetIds" ${lowestHPID}
			/return ${lowestHPID}
		}
	}
/return FALSE

Sub GetLOSPullTargetId
	/declare i int local
	/declare spawnIndex int local
	/for i 1 to ${LoSPullTargets.Size}
		/if (${Bool[${LoSPullTargets[${i}]}]}) {
			/if (${DebugMode}) /echo Evaluating ${LoSPullTargets[${i}]}
			/for spawnIndex 1 to 3
				/if (${DebugMode}) /echo evaluating los pull target ${LoSPullTargets[${i}]}
				/varset PullTargetEvaluationId ${NearestSpawn[${spawnIndex}, NPC radius ${LoSPullRadius} ${LoSPullTargets[${i}]}].ID}
				/if (${PullTargetEvaluationId}) {
					/call ValidateTarget ${PullTargetEvaluationId} TRUE
					/if (${Macro.Return}) {
						/if (${DebugMode}) /echo Spawn type is good
						/if (${Spawn[id ${PullTargetEvaluationId}].LineOfSight}) {
							/if (${DebugMode}) /echo pulling ${Spawn[id ${PullTargetEvaluationId}].CleanName}
							/varset PullTargetDistance ${Spawn[id ${PullTargetEvaluationId}].Distance}
							/varset PullTargetId ${PullTargetEvaluationId}
							/next i
						} else {
							/if (${DebugMode}) /echo target ${PullTargetEvaluationId} is not in LOS
						}
					}
				}
			/next spawnIndex
		}
	/next i
/return ${PullTargetId}


Sub EvaluateAETargets
	/if (${DebugMode}) /echo EvaluateAETargets
	
	/declare cumulativeX float local 0
	/declare cumulativeY float local 0
	/declare analyzedTargetCount int local 0
	
	/declare i int local
	
	/for i 1 to ${Me.XTarget}
		/if (${Me.XTarget[${i}].Type.Equal[NPC]}) {
			/if (${Spawn[id ${Me.XTarget[${i}].ID}].Distance} <= 50) {
			
				/varcalc cumulativeY ${cumulativeY} + ${Spawn[id ${Me.XTarget[${i}].ID}].Y}
				/varcalc cumulativeX ${cumulativeX} + ${Spawn[id ${Me.XTarget[${i}].ID}].X}
				/varcalc analyzedTargetCount ${analyzedTargetCount} + 1
			}
		}
	/next i
	
	/if (${analyzedTargetCount} == 0) /return 0
	
	/varcalc AECentroidX ${cumulativeX} / ${analyzedTargetCount}
	/varcalc AECentroidY ${cumulativeY} / ${analyzedTargetCount}
/return ${CcTargetsInRange}


Sub TargetSync(int targetId)
	/if (${DebugMode}) /echo TargetSync ${targetId} - current target id = ${Target.ID}
	/if (${targetId} == 0) /return FALSE
	
	/target id ${targetId}
	/delay 1s ${Target.ID} == ${targetId}
/return ${Target.ID} == ${targetId}

Sub ValidateTarget(int targetId, bool onlyLos)
	/if (${DebugMode}) /echo ValidateTarget ${targetId} - only los: ${Bool[${onlyLos}]}
	/if (${DebugMode}) /echo Target Distance = ${Spawn[id ${targetId}].Distance} - pull target distance = ${PullTargetDistance}
	/if (${Spawn[id ${targetId}].Type.Equal[Corpse]}) /return FALSE
	/if (!${Spawn[id ${targetId}].ID}) {
		/if (${DebugMode}) /echo spawn doesn't exist
		/call ClearPullTarget
	}
	
	/declare los bool local ${Spawn[id ${targetId}].LineOfSight}
	
	/if (${PullMode} && ${onlyLos} && !${los}) {
		/echo Target ${targetId} is invalid because it's not in LOS
		/return FALSE
	}
	
	/if (${Me.GroupMarkNPC[2].ID} && ${targetId} == ${Me.GroupMarkNPC[2].ID}) {
		/echo ${targetId} marknpc2 is set, but it's not the target
		/return FALSE
	}

	/if (${Spawn[id ${targetId}].PctHPs} >= 100 && ${Bool[${onlyLos}]} && !${los} && !${PullMode}) {
		/if (${DebugMode}) /echo ${targetId} is not in los
		/return FALSE	
	}	
	
	/if (${Spawn[id ${targetId}].Distance} > ${PullTargetDistance}) {
		/if (${DebugMode}) /echo ${targetId} is too far away ${Spawn[id ${targetId}].Distance} > ${PullTargetDistance}
		/if (${PullTargetDistance} == 0) /call ClearPullTarget
		/return FALSE	
	}
	
	/if (!${Me.XTarget[1].ID} && !${PullMode}) {
		/if (${Bool[${onlyLos}]} && !${los}) {
			/if (${DebugMode}) /echo ${targetId} is bad because of LOS
			/call ClearPullTarget
			/return FALSE
		}
	}
	
	/if (!${Spawn[id ${targetId}].Type.Equal[NPC]}) {
		/if (${DebugMode}) /echo ${targetId} is not an NPC
		/call ClearPullTarget
		/return FALSE	
	}
	
	/if (${Spawn[id ${targetId}].Distance} > ${MaxTargetDistance}) {
		/if (${DebugMode}) /echo ${targetId}'s distance (${Spawn[id ${targetId}].Distance}) is outside of the MaxTargetDistance (${MaxTargetDistance})
		/call ClearPullTarget
		/return FALSE
	}

	/if (!${IsTank} && ${Spawn[id ${targetId}].PctHPs} > ${TargetHpEngagePercentage}) {
		/if (${DebugMode}) /echo target ${targetId}'s PctHPs are too high to engage ${Spawn[id ${targetId}].PctHPs} vs ${TargetHpEngagePercentage}
		/call ClearPullTarget
		/return FALSE
	}
		
	/declare i int local
	/for i 1 to ${BadTargetIds.Size}
		/if (${Bool[BadTargetIds[${i}]]} && ${BadTargetIds[${i}]} == ${targetId}) {
			/if (${DebugMode}) /echo target is a badTargetId
			/call ClearPullTarget
			/return FALSE
		}
	/next i
	
	/varset PullTargetDistance ${Spawn[id ${targetId}].Distance}
	/varset PullTargetId ${targetId}
	
	/if (${DebugMode}) /echo Target ${targetId} is valid
/return TRUE


Sub CountTargetsInRange(int range)
	/if (${DebugMode}) /echo CountTargetsInRange
	/declare targetsInRange int local
	/declare i int local
		
	/for i 1 to ${Me.XTarget}
		/if (${Me.XTarget[${i}].Type.Equal[NPC]} && ${Spawn[id ${Me.XTarget[${i}].ID}].Distance} <= ${range}) {
			/varcalc targetsInRange ${targetsInRange} + 1
		}
	/next i
/return ${targetsInRange}

