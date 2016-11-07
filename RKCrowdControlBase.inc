#Event MezBroken "#*#has been awakened by #1#"
#Event MezImmune "#*#Your target cannot be mesmerized.#*#"
#Event CharmImmune "#*#cannot be charmed#*#"

Sub Event_MezBroken(eventText, assHole)
	/docommand /${ChatCommand} "Mez was broken by ${assHole}"
	/echo Mez was broken by ${assHole}
	/call ClearMezzedMobArrays
/return

Sub Event_MezImmune
	/docommand /${ChatCommand} "${Spawn[id ${CurrentMezTargetId}].CleanName} is unmezzable, adding to unmezzable mobs"
	/call AddToArray "MezImmuneMobs" "${Spawn[id ${CurrentMezTargetId}].CleanName}"
/return

Sub Event_CharmImmune
	/docommand /${ChatCommand} "${Spawn[id ${CurrentCharmTargetId}].CleanName} is uncharmable, switching to nocharm mode"
	/call AddToArray "CharmImmuneMobs" "${Spawn[id ${CurrentCharmTargetId}].CleanName}"
	/varset ShouldICharm FALSE
/return

Sub Init_RKCrowdControlBase
	/declare CCMode bool outer FALSE
	/declare MaxCrowdControlDistance int outer 110

	/declare CurrentCharmTargetId int outer
	/declare CurrentMezTargetId int outer

	/declare SingleTargetMez string outer Lullaby of the Forsaken Rk. II
	/declare SingleTargetMezLevelCap int outer 98	

	/declare SingleTargetMez2 string outer
	/declare SingleTargetMez2LevelCap int outer 98	

	/declare AEMezSpell string outer
	/declare AEMezSpellLevelCap int outer 98	
	
	/declare AEMezSub string outer
	/declare MezSub string outer	

	/declare ShouldICharm bool outer TRUE
	/declare CharmMode bool outer FALSE
	/declare CharmSpell string outer
	/declare CharmSpellLevelCap int outer 87	
	
	/call CreateDictionary "MezzedMobTimers" 20
	/declare MezImmuneMobs[50] string outer

	/declare CharmImmuneMobs[50] string outer	
	
	/call AddToArray "NonCombatSubs" "ClearMezzedMobArrays"
	
	/call AddToArray "PreTargetAcquisitionSubs" "ExecuteCrowdControl"
	
	/echo Done initializing crowd control base!
/return

Sub ExecuteCrowdControl
	/if (${DebugMode}) /echo Execute Crowd Control
	/if (${Group.MainAssist.ID} == ${Me.ID}) /return FALSE
	/if (!${Bool[${Me.XTarget[1].ID}]} || !${Bool[${Me.XTarget[1].ID}]}) {
		/if (${DebugMode}) /echo ExecuteCrowdControl Early Escape
		/return FALSE
	}
		
	/if (${Bool[${AEMezSpell}]}) {
		/if (${Me.GemTimer[${AEMezSpell}].TotalSeconds} == 0) {
			/call EvaluateAETargets
		
			/if (${Macro.Return} > 2) {
				/if (${DebugMode}) /echo Attempting to AE Mez
				
				/if (${Bool[${AEMezSub}]}) {
					/call ${AEMezSub}
					/if (${Macro.Return}) /return TRUE
				}
				
				/call RKCast "${AEMezSpell}" ${CurrentTargetId}
				/if (${Macro.Return}) /return TRUE
			}
		}
	}

	/if (!${CCMode}) /return FALSE
	
	/declare ccTargetId int local 0	
	/declare mezTimer int local 0
	/declare mainTargetId int local 0
	
	/if (${DebugMode}) /echo checking single target CC

	/call GetMainAssistTargetID
	/varset mainTargetId ${Macro.Return}
	/declare i int local 0
	/for i 1 to ${Me.XTarget}
		/if (${DebugMode}) /echo checking xtarget ${i}
		/varset ccTargetId ${Me.XTarget[${i}].ID}
		/if (${Me.XTarget[${i}].ID} != ${CurrentTargetId}) {
			/call IsMobMezzable ${ccTargetId}
			/if (${Macro.Return}) {
				/if (${Spawn[id ${ccTargetId}].Level} <= ${SingleTargetMezLevelCap}) {
					/if (${ccTargetId} != ${mainTargetId}) {
						/call GetCrowdControlTimer ${ccTargetId}
						/varset mezTimer ${Macro.Return}
						
						/if (${mezTimer} < 3000) {
							/call TargetSync ${ccTargetId}
							/delay 5 ${Bool[${Target.Buff}]}
							
							/if (${Target.Buff[${SingleTargetMez}].ID} && ${Target.Buff[${SingleTargetMez}].Duration.TotalSeconds} > 3) {
								/varcalc mezTimer ${MacroQuest.Running} + (${Target.Buff[${SingleTargetMez}].Duration.TotalSeconds} * 1000)
								/if (${DebugMode}) /echo target is mezzed - adding timer to dictionary id ${ccTargetId} - ${mezTimer}
								/call AddToDictionary "MezzedMobTimers" "${ccTargetId}" ${mezTimer}
								/docommand /${ChatCommand} "CCd ${Target.CleanName}"
								|/g CCd ${Target.CleanName}
							} else {
								/if (${DebugMode}) /echo target needs to be CCd
								/call CrowdControlTarget ${ccTargetId}
								/if (${Macro.Return}) /return TRUE
							}
						}
					}
				}
			}
		}
	/next i

	/if (${twisting}) /twist start
/return FALSE

Sub GetCharmTarget
	/if (${Me.GroupMarkNPC[2].ID}) {
		/if (${Me.GroupMarkNPC[2].PctHPs} > 30) /return ${Me.GroupMarkNPC[2].ID}
	}
	
	/declare highestPct int local 0
	/declare highestId int local 0
	/declare i int local 0	
	/for i 1 to ${Me.XTarget}
		/if (!${Me.XTarget[${i}].Named} && ${Me.XTarget[${i}].PctHPs} > ${highestPct}) {
			/if (${Me.XTarget[${i}].Level} <= ${CharmSpellLevelCap} && ${Me.XTarget[${i}].PctHPs} > 50) {
				/call IsMobCharmable ${Me.XTarget[${i}].ID}
				/if (${Macro.Return}) {
					/varset highestPct ${Me.XTarget[${i}].PctHPs}
					/varset highestId ${Me.XTarget[${i}].ID}
				}
			}
		}
	/next i
	/if (${DebugMode}) /echo ${highestId} is best charm target with ${highestPct} % hps
	/varset CurrentCharmTargetId ${highestId}
/return ${highestId}

Sub ClearMezzedMobArrays
	/call ClearDictionary "MezzedMobTimers"
/return FALSE

Sub IsMobCharmable(int targetId)
	/if (!${Spawn[id ${targetId}].Type.Equal[NPC]}) /return FALSE
	/call GetStringArrayIndex "CharmImmuneMobs" "${Spawn[id ${targetId}].CleanName}"
	/if (${Bool[${Macro.Return}]}) /return FALSE
/return ${Spawn[id ${targetId}].LineOfSight}

Sub IsMobMezzable(int targetId)
	/if (!${Spawn[id ${targetId}].Type.Equal[NPC]}) /return FALSE
	/call GetStringArrayIndex "MezImmuneMobs" "${Spawn[id ${targetId}].CleanName}"
/return !${Bool[${Macro.Return}]}

Sub GetCrowdControlTimer(int targetId)
	/declare mezTimer int local 0
	/call GetFromOrAddToDictionary "MezzedMobTimers" "${targetId}" 0
	/varcalc mezTimer ${Int[${Macro.Return}]} - ${MacroQuest.Running}
	/if (${DebugMode}) {
		/declare mezTimerInSeconds float local ${mezTimer} / 1000
		/echo Mez timer for TargetId ${ccTargetId} - ${Spawn[id ${ccTargetId}].CleanName} is ${mezTimerInSeconds}
	}
/return ${mezTimer}

Sub CrowdControlTarget(int targetId)
	/if (${DebugMode}) /echo CrowdControlTarget ${targetId}
	/if (!${Spawn[id ${targetId}].Distance} > ${Spell[${SingleTargetMez}].MyRange}) /return FALSE
	/attack off
	/if (${Bool[${MezSub}]}) {
		/call ${MezSub} ${targetId}
		/if (${Macro.Return}) /return TRUE 
	}
		
	|/if (!${Me.Casting.Name.Equal[${AEMezSpell}]} && !${Me.Casting.Name.Equal[${SingleTargetMez}]} && !${Me.Casting.Name.Equal[${SingleTargetMez2}]}) {
	|	/echo stopcast 1
	|	/return FALSE
	|	/stopcast
	|	/delay 1s
	|}
	
	/call RKDebuffCast "${SingleTargetMez}" "${SingleTargetMez}" ${targetId}
	/if (${Macro.Return}) {
		/varset CurrentMezTargetId ${targetId}
		/if (${Me.Class.Name.Equal[Bard]}) {
			/echo stopcasting 2
			/delay 4s !${Me.Casting.ID}
			/stopcast
		}
		/target clear
		/return TRUE
	}
	
	/if (${Bool[${SingleTargetMez2}]}) {	
		/call RKDebuffCast "${SingleTargetMez2}" "${SingleTargetMez2}" ${targetId}
		/if (${Macro.Return}) {
			/varset CurrentMezTargetId ${targetId}
			/if (${Me.Class.Name.Equal[Bard]}) {
				/delay 4s !${Me.Casting.ID}
				/echo stopcasting 3
				/stopcast
			}
			/target clear
		}
	}
		
/return ${Macro.Return}
