Sub CheckPosition
	/if (!${IsTank} || !${KeepAgroOnAdds}) /return FALSE
	/if (!${Bool[${Me.XTarget[1].ID}]}) /return FALSE
	/if (${BOTMode} == 5) /return FALSE

	|/if (${StickTimer} > 0) /return FALSE
	|/varset StickTimer 10
	
	
	|/if (${AECentroidX} != 0 && ${AECentroidY} != 0) /call FaceAECentroid
	
	/declare needToMove bool local FALSE	
	/declare i int local
	
	/call FaceAECentroid

	/for i 1 to ${Me.XTarget}
		/if (${Me.XTarget[${i}].Type.Equal[NPC]}) {
			/if (${Me.XTarget[${i}].Distance} < 2) {
				/if (${DebugMode}) /echo Target is too close, punching him back up
				/call RKAACast "Press the Attack" ${Me.XTarget[${i}]}
				| TODO: move this to warrior
				/keypress s hold
				/delay 1
				/keypress s
				/delay 1
			}

			/if (${Me.XTarget[${i}].Distance} > 25) {
				/if (${DebugMode}) /echo Target is too far, sticking to them for a sec
				/call SetStickTarget ${Me.XTarget[${i}].ID} ${StickDistance} ${StickFlags}
				/delay 1s ${Me.XTarget[${i}].Distance} < 25
				/call ClearStickTarget
				/delay 1
				/call FaceAECentroid
			}

			
			/call AmIFacingTarget ${Me.XTarget[${i}].ID} 90
			/if (!${Macro.Return}) {
				/varset needToMove TRUE
				/if (${DebugMode}) /echo needToMove - I'm not facing target
			} else {
				/if (${IsTank}) {
					/call IsTargetFacingMe ${Me.XTarget[${i}].ID}
					/if (!${Macro.Return}) {
						/xt ${i}
						/call SetStickTarget ${Target.ID} ${StickDistance} ${StickFlags}
						/return FALSE
					}
				}
			}
		}
	/next i
	
	/declare randomValue int local ${Math.Rand[3]}
	
	/if (!${needToMove}) {
		/varset MoveTimer 30
	} else {
		/if (${MoveTimer} == 0) {
			/if (${DebugMode}) /echo need to move
			/varset MoveTimer 30
			/call FaceAECentroid					
			/delay 4
						
			/if (${TankId} == ${Me.ID} && ${Me.XTarget[2].ID}) {
				/if (${randomValue} == 0) {
					/if (${DebugMode}) /echo rand movement backwards
					/keypress s
					/keypress s
				} else /if (${randomValue} == 1) {
					/if (${DebugMode}) /echo rand movement strafe right
					/if (${Stick.Active}) {
						/call ClearStickTarget
						/if (${DebugMode}) /echo stick off 5
					}
					/keypress e hold
					/delay 4
					/keypress e
					/call FaceAECentroid
					/keypress e
					/delay 2
					/keypress e
				} else {
					/if (${DebugMode}) /echo rand movement strafe left
					/if (${Stick.Active}) {
						/call ClearStickTarget
						/if (${DebugMode}) /echo stick off 5
					}
					/keypress q hold
					/delay 4
					/keypress q
					/call FaceAECentroid
					/keypress q
					/delay 2
					/keypress q

				
				
					|/echo fallthrough needToMove AE centroid movement			
					|/keypress s hold
					|/delay 2
					|/keypress s		
					|/delay 2
					|/keypress e hold
					|/delay 3
					|/keypress e
				}

				/call FaceAECentroid
			
				/delay 2
				/return FALSE
			} else {
				/if (${InCombatStickToCommander} || ${BOTMode} == 1) {
					/if (${BOTMode} !=2 && ${BOTMode} !=4) {
						/if (!${Stick.Active}) {
							/if (${DebugMode}) /echo SetStickTarget 7
							/call SetStickTarget ${RKCommanderId} 5
							/if (${DebugMode}) /echo facing target 2
							/face fast nolook
						}
					}
				}
			}
		}
	}
	
	/if (${Target.Distance} > ${StickDistance}) {
		/if (${DebugMode}) /echo SetStickTarget 5 
		/call SetStickTarget ${Target.ID} ${StickDistance}
	}
	
/return FALSE


Sub CalculateMobsCentroid
	/if (${DebugMode}) /echo CalculateMobsCentroid
	/declare i int local
		
	/declare cumulativeX float local 0
	/declare cumulativeY float local 0
	/declare analyzedTargetCount int local 0

	/for i 1 to ${Me.XTarget}
		/if (${Me.XTarget[${i}].Type.Equal[NPC]}) {
			/if (${Spawn[id ${Me.XTarget[${i}].ID}].Distance} <= ${MaxTargetDistance}) {
				/varcalc cumulativeX ${cumulativeX} + ${Spawn[id ${Me.XTarget[${i}].ID}].X}
				/varcalc cumulativeY ${cumulativeY} + ${Spawn[id ${Me.XTarget[${i}].ID}].Y}			
			}
			/varcalc analyzedTargetCount ${analyzedTargetCount} + 1
		}
	/next i
	
	/varcalc MobsCentroidX ${cumulativeX} / ${analyzedTargetCount}
	/varcalc MobsCentroidX ${cumulativeX} / ${analyzedTargetCount}
	/varcalc MobsCentroidY ${cumulativeY} / ${analyzedTargetCount}	
/return FALSE


Sub AmIFacingTarget(int targetId, int tolerance)
	/if (${DebugMode}) /echo AmIFacingTarget ${targetId} ${Spawn[id ${targetId}].CleanName}
	
	/declare desiredHeading float local ${Me.HeadingToLoc[${Spawn[id ${targetId}].Y}, ${Spawn[id ${targetId}].X}].DegreesCCW}
	/declare deltaFromDesiredHeading float local ${Math.Calc[${desiredHeading} - ${Me.Heading.DegreesCCW}]}
	
	/if (${DebugMode}) /echo ${Spawn[id ${targetId}].CleanName} is at angle ${deltaFromDesiredHeading}
	/if (${DebugMode}) /echo  Desired Heading = ${desiredHeading} my heading = ${Me.Heading.DegreesCCW}
	
	/if (${BOTMode} != 5 && ${Math.Abs[${deltaFromDesiredHeading}]} > ${tolerance}) {
		/if (${DebugMode}) /echo ${targetId} isn't infront of me, my angle is off by ${deltaFromDesiredHeading}
		/return FALSE
	}
/return TRUE

Sub IsTargetFacingPet(int targetId)
	/if (${DebugMode}) /echo IsTargetFacingPet
	/declare desiredHeading float local ${Spawn[id ${targetId}].HeadingToLoc[${Me.Pet.Y}, ${Me.Pet.X}].DegreesCCW}
	/if (${DebugMode}) /echo desiredHeading ${desiredHeading}
	/declare deltaFromDesiredHeading float local ${Math.Calc[${Math.Abs[${desiredHeading} - ${Spawn[id ${targetId}].Heading.DegreesCCW}	]}]}

	/if (${BOTMode} != 5 && ${deltaFromDesiredHeading} > 9) {
		/if (${DebugMode}) /echo target id ${targetId} isn't facing ${Me.Pet.Name} correctly - his angle is off by ${deltaFromDesiredHeading}
		/if (${DebugMode}) /echo target id ${targetId} angle to pet is ${deltaFromDesiredHeading}
		/return FALSE
	}
/return TRUE


Sub IsTargetFlankingMe(int targetId)
	/if (${DebugMode}) /echo IsTargetFlankingMe ${targetId} ${Spawn[id ${targetId}].CleanName}
	
	/declare desiredHeading float local ${Me.HeadingToLoc[${Spawn[id ${targetId}].Y}, ${Spawn[id ${targetId}].X}].DegreesCCW}
	/declare deltaFromDesiredHeading float local ${Math.Abs[${Math.Calc[${desiredHeading} - ${Me.Heading.DegreesCCW}]}]}

	/if (${DebugMode}) /echo ${Spawn[id ${targetId}].CleanName} is at angle ${deltaFromDesiredHeading} - Desired Heading = ${desiredHeading} my heading = ${Me.Heading.DegreesCCW}

	
	/if (${deltaFromDesiredHeading} > ${tolerance}) {
		/if (${DebugMode}) /echo ${targetId} isn't infront of me, my angle is off by ${deltaFromDesiredHeading}
		/return FALSE
	}
/return TRUE

Sub AreTargetsFacingMe
	/if (!${Me.XTarget[1].ID}) /return TRUE
	/declare i int local
	/for i 1 to ${Me.XTarget}
		/call IsTargetFacingMe ${Me.XTarget[${i}].ID}
		/if (!${Macro.Return}) /return FALSE
	/next i
/return TRUE

Sub AreTargetsFacingPet
	/if (!${Me.XTarget[1].ID}) /return TRUE
	/declare i int local
	/for i 1 to ${Me.XTarget}
		/if (${Me.XTarget[${i}].Type.Equal[NPC]}) {
			/call IsTargetFacingPet ${Me.XTarget[${i}].ID}
			/if (!${Macro.Return}) /return FALSE
		}
	/next i
/return TRUE

Sub IsTargetFacingMe(int targetId)
	/if (${DebugMode}) /echo IsTargetFacingMe
	
	/if (${Me.Class.Name.Equal[Magician]} && ${Me.Pet.ID}) {
		/call IsTargetFacingPet ${targetId}
		/return ${Macro.Return}
	}
	
	/declare desiredHeading float local ${Spawn[id ${targetId}].HeadingToLoc[${Me.Y}, ${Me.X}].DegreesCCW}
	/if (${DebugMode}) /echo desiredHeading ${desiredHeading}
	/declare deltaFromDesiredHeading float local ${Math.Calc[${Math.Abs[${desiredHeading} - ${Spawn[id ${targetId}].Heading.DegreesCCW}	]}]}

	/if (${BOTMode} != 5 && ${deltaFromDesiredHeading} > 9) {
		/if (${DebugMode}) /echo target id ${targetId} isn't facing me correctly - his angle is off by ${deltaFromDesiredHeading}
		/return FALSE
	}
/return TRUE


Sub SetStickTarget(int targetId, int distance)
	/if (${DebugMode}) /echo SetStickTarget ${targetId} ${distance}
	/if (${targetId} == 0) /return FALSE
	/if (${Spawn[id ${targetId}].Distance} <= ${distance}) /return FALSE

	/if (${DebugMode}) /echo setting stick target - CurrentID = ${CurrentStickTargetId} - new ID = ${targetId} - distance = ${distance}
	
	/if (${Me.Casting.ID} && !${Me.Class.Name.Equal[Bard]}) /return FALSE	
	
	/if (${CurrentStickTargetId} == ${targetId}) {
		/if (${StickTimer} > 0) /return FALSE
		/if (${DebugMode}) /echo Timer elapsed - set stick to ${targetId}
		/varset StickTimer 10
		/if (!${Stick.Active}) /stick id ${targetId} ${distance} ${StickFlags} 
		/return FALSE
	}
	
	/varset CurrentStickTargetId ${targetId}
	/if (${Stick.Active}) {
		/stick off
		/delay 1
	}
	/stick id ${targetId} ${StickFlags} ${distance}
	/echo stuck to ${targetId}
	
	/if (!${Bool[${distance}]} || ${distance} == 0) /varset distance 15

	/if (${Spawn[id ${targetId}].Type.Equal[NPC]}) {
		/echo sticking to ${targetId} ${Spawn[id ${targetId}].CleanName} at distance ${distance} with flags ${StickFlags}
		/stick id ${targetId} ${StickFlags} ${distance}
	} else {
		/echo sticking to ${targetId} ${Spawn[id ${targetId}].CleanName} at distance ${distance}
		/stick id ${targetId} ${distance}
	}	
/return FALSE

Sub CheckCommanderFollow
	/if (${DebugMode}) /echo CheckCommanderFollow - BOTMode = ${BOTMode} | Commander = ${RKCommander} | XTarget = ${Me.XTarget}
	/if (${RKCommanderId} == ${Me.ID}) /return FALSE
	/if (${BOTMode} == 2 || ${BOTMode} == 4) /return FALSE

	/if (${BOTMode} == 1) {
		/if (${DebugMode}) /echo SetStickTarget 4 - invis commander follow
		/call SetStickTarget ${RKCommanderId} 5
		/return FALSE
	}
	
	/if (${Bool[${Me.XTarget[1].ID}]}) {
		/if (${InCombatStickToCommander} || ${Spawn[id ${TankId}].Distance} > 30 || ${BOTMode} == 1) {
			/if (!${Stick.Active}) {
				/if (${DebugMode}) /echo SetStickTarget 2
				/call SetStickTarget ${RKCommanderId} 5
			}
		} else {
			/if (${Stick.Active} && ${CurrentStickTargetId} == ${RKCommanderId}) {
				/call ClearStickTarget
				/if (${DebugMode}) /echo stick off 8
			}
		}
		/return FALSE
	}
	
	/if (${BOTMode} == 1 || ${BOTMode} == 3) {
		/if (!${Stick.Active} && ${Spawn[id ${RKCommanderId}].Distance} > 10) {
			/if (${DebugMode}) /echo SetStickTarget 3
			/call SetStickTarget ${RKCommanderId} 5
		}
	}
/return FALSE

Sub ClearStickTarget
	|/if (${DebugMode}) 
	/echo ClearStickTarget
	/if (${CurrentStickTargetId} != 0) /varset CurrentStickTargetId 0
	/if (${CurrentStickTargetId} == 0 && !${Stick.Active}) /return TRUE
	/stick off
	/if (${DebugMode}) /echo stick off 9
/return TRUE