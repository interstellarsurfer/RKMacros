Sub Init_RKMeleeBase
	/declare CombatAbilities[${DynamicArraySize}] string outer
	/declare CombatDiscs[${DynamicArraySize}] string outer	
	/declare BurnDiscs[${DynamicArraySize}] string outer
	
	/declare MeleeMode bool outer TRUE
	
	/call AddToArray "NonCombatSubs" "MeleeNonCombat"
	/call AddToArray "CombatActionSubs" "MeleeSubs"
	/call AddToArray "BurnCombatActionSubs" "ExecuteBurnDiscs"
	/call AddToArray "PreTargetAcquisitionSubs" "CheckNoTargetMelee"

	/varset InCombatStickToCommander FALSE
	
	/echo Done initializing melee base!
/return

Sub MeleeNonCombat
	|/if (${Me.Haste} < 170 && !${Me.Buff[Resurrection Sickness].ID} && !${Me.Buff[Revival Sickness].ID} && !${Bool[${Me.Hasted}]} && !${Bool[${Me.Slowed}]}) /cast item "Distillate of Alacrity X"
/return FALSE

Sub CheckNoTargetMelee
	/if (${BOTMode} != 5 && !${Me.XTarget[1].ID} && !${Target.ID}) {
		/attack off
	}
/return FALSE

Sub MeleeSubs
	/if (${DebugMode}) /echo MeleeSubs
	/if (!${MeleeMode}) /return FALSE
	/if (!${Target.ID}) /return FALSE
	/if (${BOTMode} <= 2) /return FALSE
	/if (!${Target.Type.Equal[NPC]} && !${Target.Type.Equal[Object]}) /return FALSE
	
	/if (${Target.Distance} < ${MaxMeleeDistance}) /attack on
	/if (${BOTMode} != 5) {
		/attack on
		/if (${DebugMode}) /echo calling CheckTargetDistance ${CurrentTargetId}
		/call CheckTargetDistance ${CurrentTargetId}
		/if (!${Target.ID}) /return FALSE
	
		/if (${AECentroidX} != 0 && ${AECentroidY} != 0) {
			/if (${IsTank} && ${KeepAgroOnAdds} && ${Bool[${Me.XTarget[2].ID}]}) {
				/if (${Stick.Active} && ${Target.Distance} < ${MaxMeleeDistance}) {
					/stick off
					/if (${DebugMode}) /echo stick off 3
				}
				|/echo facing AE loc
				/call FaceAECentroid
			
				/call DistanceToCentroid ${AECentroidY},${AECentroidX}
				|/echo Distance to Loc = ${Macro.Return}
				|/echo I'm ${Macro.Return} away from AECentroid ${AECentroidY},${AECentroidX}  my loc is ${Me.Y} ${Me.X}				
				/if (${Macro.Return} < 2) {
					/if (${DebugMode}) /echo Backing up for AE Centroid
					/keypress s hold
					/delay 2
					/keypress s		
					/delay 2
					/keypress e hold
					/delay 3
					/keypress e
				}
				
				/keypress HOME
			} else {
				/if (${IsTank} && !${KeepAgroOnAdds} && ${Stick.Active} && ${Target.Type.Equal[NPC]} && ${Target.Distance} < ${MaxMeleeDistance}) {
					/if (${DebugMode}) /stick off
					/echo stick off 4
				}
				/if (${Target.ID}) /face fast nolook
			}
		}
	}
	/call ExecuteCombatAbilities
/return FALSE

Sub DistanceToCentroid
	/return ${Math.Sqrt[${Math.Calc[${Math.Calc[${Math.Abs[${AECentroidX} - ${Me.X}]} ^ 2]} + ${Math.Calc[${Math.Abs[${AECentroidY} - ${Me.Y}]} ^ 2]}]}]}
return 0

Sub CheckTargetDistance(int targetId)
	/if (${DebugMode}) /echo CheckTargetDistance
	/if (${Me.LAHoTT} > 0 && !${IsTank} && ${Me.TargetOfTarget.ID} != ${TankId}) {
		/if (!${Me.Name.Equal[${RKCommander}]} && !${Me.TargetOfTarget.Type.Equal[Pet]}) {
			/if (${DebugMode}) /echo Escaping CheckTargetDistance because of target's target
			/return FALSE
		}
	}

	/if (${BOTMode} <= 2) /return FALSE
		
	/if (${Spawn[${targetId}].Distance} < 20) {
		/if (${Spawn[${targetId}].Z} < (${Me.Z} - 5)) {
			/if (!${Descending}) {
				/if (${DebugMode}) /echo target: ${Spawn[${targetId}].Z} < me: ${Me.Z} - Descending
				/keypress end hold
				/varset Descending TRUE
			}
		} else {
			/if (${Descending}) {
				/keypress end
				/varset Descending FALSE
			}
		}
	}

	/if (${Stick.Active}) /return FALSE
	/if (!${Target.Type.Equal[NPC]} && !${Target.Type.Equal[Object]}) /return FALSE
	
	/if (${Spawn[${targetId}].Distance} > ${MaxMeleeDistance}) {
		/call RKAACast "Grappling Strike" ${targetId}
		|/keypress w hold
		/delay 1
		|/keypress w
		|/call SetStickTarget ${targetId} ${StickDistance}
	}
	
	/if (!${IsTank} || !${SetStickTarget}) {
		/if (${DebugMode}) /echo SetStickTarget 1 
		/call SetStickTarget ${targetId} ${StickDistance} ${StickFlags}
	}
/return FALSE

Sub ExecuteCombatAbilities
	/if (${DebugMode}) /echo ExecuteCombatAbilities

	/if (${Target.Distance} > 25) /return FALSE
	
	/declare i int local
	/if (${Me.Casting.ID} && !${Me.Class.Name.Equal[Bard]}) /return FALSE
	/if (${Spawn[id ${CurrentTargetId}].Type.Equal[NPC]}) {
		/alt act 15073
		|/call RKAACast "Banestrike" ${CurrentTargetId}
	}
	
	/declare currentAbility string local
	/for i 1 to ${CombatAbilities.Size}
		/varset currentAbility ${CombatAbilities[${i}]}
		/if (${Bool[${currentAbility}]} && ${currentAbility.Length} > 0) {
			/if (${DebugMode}) /echo using ability ${currentAbility}
			
			/if (${AltAbility[${currentAbility}].ID}) {
				/call RKAACast "${currentAbility}" ${CurrentTargetId}
				/next i
			}
			
			/if (${currentAbility.Equal[Taunt]}) {
				/if (${TauntMode} && (${Me.LAHoTT} == 0 || ${Me.TargetOfTarget.ID} != ${TankId})) {
					/doability "Taunt"
				}
				/return FALSE
			}
			
			/doability ${Me.Ability[${currentAbility}]}		
		}
	/next i

	/if (${Me.PctEndurance} > 5) {
		/for i 1 to ${CombatDiscs.Size}
			/if (${Bool[${CombatDiscs[${i}]}]}) {
				/call RKDisc "${CombatDiscs[${i}]}"
			}
		/next i	
	}
/return FALSE

Sub ExecuteBurnDiscs
	/if (${DebugMode}) /echo ExecuteBurnDiscs
	
	/if (${Me.AAPoints} > 50 && ${GlyphMode}) {
		/if (!${Me.AltAbility[Glyph of Destruction II]}) {
			/echo buying melee glyph of destruction II
			/alt buy 7019
		}
		/call RKAACast "Glyph of Destruction II" ${CurrentTargetId}
		/if (${Macro.Return}) /docommand /${ChatCommand} "Running Glyph of Destruction II"
	}
	
	/declare i int local
	/for i 1 to ${BurnDiscs.Size}
		/if (${Bool[${BurnDiscs[${i}]}]}) {
			/call RKDisc "${BurnDiscs[${i}]}"
		/next i
	}
/return FALSE