-- npc id Prince Liam
-- no previous scriptname
UPDATE `creature_template` SET `ScriptName`='npc_fl_skip_worg' WHERE  `entry`=34850;

-- npc id Sassy Hardwrench
-- no previous scriptname
UPDATE `creature_template` SET `ScriptName`='npc_fl_skip_gob' WHERE  `entry`=34668;

-- npc id lich king
-- Original npcflag was 2
-- no previous scriptname
UPDATE `creature_template` SET `npcflag`='3', `ScriptName`='npc_fl_skip_lich' WHERE  `entry`=25462;
