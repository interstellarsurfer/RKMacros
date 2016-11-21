Sub Init_RKHealerBase
	/declare HealMode bool outer TRUE

	/declare HealSpell string outer
	/declare TankHealThreshold int outer 90
	/declare SingleTargetHealThreshold int outer 80

	/declare EmergencyHealSpell string outer
	/declare EmergencyHealThreshold int outer 50

	/declare GroupHeal string outer
	/declare GroupHealThreshold int outer 90
	
	/declare EmergencyGroupHeal string outer
	/declare EmergencyGroupHealThreshold int outer 0
	
	/declare HoTSpell string outer
	/declare TankHoTThreshold int outer 0
	/declare SingleTargetHoTThreshold int outer 0

	/declare GroupHoT[7] int outer 0
	/declare GroupHoTPet[7] int outer 0

	/declare LowestID int outer ${Me.ID}
	
	/declare GroupHealthPercentage float outer

	/call AddToArray "NonCombatSubs" "ExecuteHealing"
	/call AddToArray "PreTargetAcquisitionSubs" "ExecuteHealing"
	
	/echo Done initializing healer base!
/return

Sub ExecuteHealing
	|/if (${DebugMode}) 
	/echo ExecuteHealing
	/if (${BOTMode} < 3) {
		/if (${DebugMode}) /echo early escape from ExecuteHealing due to BOTMode = ${BOTMode}
		/return FALSE
	}

	/call CalculateGroupHealth

	/if (!${HealMode}) /return FALSE

	/if (${Me.Moving}) /return FALSE
	
	/if (${TankId} != 0 && ${Spawn[id ${TankId}].ID}) {
		/call CheckSingleTargetHealth ${TankId} ${EmergencyHealThreshold} ${SingleTargetHoTThreshold} "${EmergencyHealSpell}"
		/if (${Macro.Return}) {
			/if (${DebugMode}) /echo Emergency healing Tank: ${Spawn[id ${TankId}]}
			/return FALSE
		}
		
		/call CheckSingleTargetHealth ${TankId} ${TankHealThreshold} ${SingleTargetHoTThreshold}	"${HealSpell}"
		/if (${Macro.Return}) {
			/if (${DebugMode}) /echo Healing Tank: ${Spawn[id ${TankId}]}
			/return FALSE
		}
	}
	
	/if (${GroupHealThreshold} <= 0 && ${EmergencyGroupHealThreshold} <= 0) /return FALSE
	
	/call CheckGroupHealth
	/if (${Macro.Return} && ${DebugMode}) /echo need to heal Group

	/call GetLowestHPGroupMember
	/call CheckSingleTargetHealth ${Macro.Return} ${SingleTargetHealThreshold} ${SingleTargetHoTThreshold} "${HealSpell}"
	/if (${Macro.Return}) {
		/if (${DebugMode}) /echo need to heal a group member ${Spawn[id ${LowestID}].Name} 
		/return FALSE
	} 

	/if (${Target.Type.Equal[PC]}) {
		/if (${DebugMode}) /echo Checking Target HP
		/call CheckSingleTargetHealth ${Target.ID} ${EmergencyHealThreshold} ${SingleTargetHoTThreshold} "${EmergencyHealSpell}"
		/if (${Macro.Return}) {
			/if (${DebugMode}) /echo Emergency healing Target: ${Spawn[id ${Target.ID}]}
			/return FALSE
		}
		
		/call CheckSingleTargetHealth ${Target.ID} 95 0	"${HealSpell}"
		/if (${Macro.Return}) {
			/if (${DebugMode}) /echo Healing Target: ${Spawn[id ${Target.ID}]}
			/return FALSE
		}
		
		/call CheckSingleTargetHealth ${Target.ID} 95 ${SingleTargetHoTThreshold} "${HealSpell}"
		/if (${Macro.Return}) {
			/if (${DebugMode}) /echo Healing Target: ${Spawn[id ${Target.ID}]}
			/return FALSE
		}
	}

	
/return FALSE

Sub GetLowestHPGroupMember
	/if (${DebugMode}) /echo GetLowestHPGroupMember
	/declare lowestLevel int local 100
	/declare i int local 
	/for i 0 to ${Group.Members}
		/if (${Bool[${Group.Member[${i}].Spawn}]} && ${Group.Member[${i}].Spawn.PctHPs} < ${lowestLevel}) {
			/varset lowestLevel ${Group.Member[${i}].PctHPs}
			/varset LowestID ${Group.Member[${i}].ID}
		}
	/next i
	/if (${DebugMode}) /echo Lowest HP member is ${Spawn[id ${LowestID}].Name} with ${Spawn[id ${LowestID}].PctHPs}% hps
	/if (${DebugMode}) /echo Lowest HP member is ${Spawn[id ${LowestID}].Name} with ${Spawn[id ${LowestID}].PctHPs}% hps
/return ${LowestID}

Sub CheckSingleTargetHealth(int targetId, int healThreshold, int HoTThreshold, string healSpell)
	/if (${DebugMode}) /echo CheckSingleTargetHealth ${targetId} - ${Spawn[id ${targetId}].CleanName} - ${healThreshold} ${HoTThreshold}
	
	/declare needToHeal bool local FALSE
	
	/if (${Bool[${healSpell}]}) {
		/if (${Spawn[${targetId}].PctHPs} < ${healThreshold}) {
			/varset needToHeal TRUE
			/if (${DebugMode}) /echo need to heal ${targetId} ${Spawn[${targetId}].PctHPs} < ${healThreshold}
			/call RKCast "${healSpell}" ${targetId}
		}
	}

	/if (${Bool[${HoTSpell}]}) {
		/if (${DebugMode}) /echo checking hots
		/if (${Spawn[${targetId}].PctHPs} < ${HoTThreshold}) {
			/call DoesTargetAlreadyHaveHoT ${targetId}
			/if (!${Macro.Return}) {
				/varset needToHeal TRUE
				/echo target doesn't have hot - ${targetId} ${Spawn[${targetId}]}
				/call RKCast "${HoTSpell}" ${targetId}
				/if (${Macro.Return}) /return ${Macro.Return}
			}
		}
	}
	
	/if (${Bool[${Spawn[${targetId}].Pet.ID}]}) {
		/if (${DebugMode}) /echo Checking pet ${Spawn[${targetId}].Pet.ID}
		/call CheckSingleTargetHealth ${Spawn[${targetId}].Pet.ID} ${healThreshold} ${HoTThreshold}
		/if (${Macro.Return}) /varset needToHeal TRUE
	}
/return ${needToHeal}

Sub DoesTargetAlreadyHaveHoT(int targetId)
	/if (${DebugMode}) /echo DoesTargetAlreadyHaveHoT ${targetId}
	/declare i int local
	/declare j int local
	/for i 0 to ${Group.Members}
		/if (${Group.Member[${i}].ID} == ${targetId}) {
			/varcalc j ${i} + 1
			/return ${GroupHoT[${j}]}
		}
	/next i
/return FALSE

Sub CheckGroupHealth
	/if (${DebugMode}) /echo CheckGroupHealth
	/if (!${Bool[${GroupHeal}]} && !${Bool[${EmergencyGroupHeal}]}) {
		/if (${DebugMode}) /echo no group heals - escaping check group health
		/return FALSE
	}

	/if (!${Bool[${Group}]}) /return FALSE
	
	/call CalculateGroupHealth
	
	/if (${Bool[${EmergencyGroupHeal}]}) {
		/if (${GroupHealthPercentage} < ${EmergencyGroupHealThreshold}) {
			/echo Group Health % = ${GroupHealthPercentage} casting Emergency Heal - ${EmergencyGroupHeal}
			/call RKCast "${EmergencyGroupHeal}" ${Me.ID}
			/if (${Macro.Return}) /return TRUE
		}
	}

	/if (${Bool[${GroupHeal}]}) {
		/if (${GroupHealthPercentage} < ${GroupHealThreshold}) {
			/echo Group Health % = ${GroupHealthPercentage} casting Group Heal - ${GroupHeal}
			/call RKCast "${GroupHeal}" ${Me.ID}
			/if (${Macro.Return}) /return TRUE
		}
	}
/return FALSE

Sub CalculateGroupHealth
	/declare currentGroupHealth int local 0
	/if (!${Bool[${Group}]}) /return ${Me.PctHPs}
	/declare i int local 
	/declare groupMemberCount int local
	/for i 0 to ${Group.Members}
		/if (${Group.Member[${i}].PctHPs} != 0) {
			/varcalc currentGroupHealth ${currentGroupHealth} + ${Group.Member[${i}].PctHPs}
			/varcalc groupMemberCount ${groupMemberCount} + 1
		}
	/next i
	/varcalc GroupHealthPercentage ${currentGroupHealth} / ${groupMemberCount}
	/if (${DebugMode}) /echo Current group health % is ${GroupHealthPercentage}
/return FALSE

Sub Event_HOT_Landed_Self
	/echo HoT landed.  Setting me as having a HoT.
	/varset GroupHoT[1] 1
/return

Sub Event_HOT_Worn_off_Self
	/echo HoT wore off.  Setting me as having no HoT.
	/varset GroupHoT[1] 0
/return

Sub Event_HOT_Landed(string Line, string targetName)
	/echo HoT landed.  Setting ${targetName} as having a HoT.
	/call SetHoTData "${targetName}" 1
/return

Sub Event_HOT_Worn_off(string Line, string targetName)
	/echo HoT wore off.  Setting ${targetName} as having no HoT.
	/call SetHoTData "${targetName}" 0
/return

Sub SetHoTData(string targetName, int value)
	/if (${DebugMode}) /echo SetHotData ${targetId} ${value}
	/declare i int local
	/declare j int local
	/for i 0 to ${Group.Members}
		/if (${Group.Member[${i}].CleanName.Equal[${targetName}]}) {
			/varcalc j ${i} + 1
			/varset GroupHoT[${j}] ${value}
	    } else /if (${Group.Member[${i}].Pet.CleanName.Equal[${targetName}]}) {
			/varcalc j ${i} + 1
			/varset GroupHoTPet[${j}] ${value}
		}
	/next i
/return