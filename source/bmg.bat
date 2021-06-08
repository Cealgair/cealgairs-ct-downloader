@echo off
rem Check if path-to-disc-root.txt exists
IF NOT EXIST path-to-disc-root.txt (
echo [!] path-to-disc-root.txt doesn't exist! Arborting...
pause
exit
)
rem Define Variables
SET /P MKW-PATH=<path-to-disc-root.txt
rem Get Files
wszst x %MKW-PATH%/Scene/UI/MenuSingle.szs --quiet
copy %MKW-PATH%\Scene\UI\MenuSingle.d\message\Common.bmg .\
rmdir /s /q MenuSingle.d
wbmgt decode Common.bmg --quiet
wctct bmg --le-code --long "%~1" >>Common.bmg.txt
.\bin\fart.exe Common.bmg.txt castle_course "Mario Circuit"
.\bin\fart.exe Common.bmg.txt farm_course "Moo Moo Meadows"
.\bin\fart.exe Common.bmg.txt kinoko_course "Mushroom Gorge"
.\bin\fart.exe Common.bmg.txt volcano_course "Grumble Volcano"
.\bin\fart.exe Common.bmg.txt factory_course "Toad's Factory"
.\bin\fart.exe Common.bmg.txt shopping_course "Coconut Mall"
.\bin\fart.exe Common.bmg.txt boardcross_course "DK Summit"
.\bin\fart.exe Common.bmg.txt truck_course "Wario's Gold Mine"
.\bin\fart.exe Common.bmg.txt beginner_course "Luigi Circuit"
.\bin\fart.exe Common.bmg.txt senior_course "Daisy Circuit"
.\bin\fart.exe Common.bmg.txt ridgehighway_course "Moonview Highway"
.\bin\fart.exe Common.bmg.txt treehouse_course "Maple Treeway"
.\bin\fart.exe Common.bmg.txt koopa_course "Bowser's Castle"
.\bin\fart.exe Common.bmg.txt rainbow_course "Rainbow Road"
.\bin\fart.exe Common.bmg.txt desert_course "Dry Dry Ruins"
.\bin\fart.exe Common.bmg.txt water_course "Koopa Cape"
.\bin\fart.exe Common.bmg.txt old_peach_gc "GCN Peach Beach"
.\bin\fart.exe Common.bmg.txt old_mario_gc "GCN Mario Circuit"
.\bin\fart.exe Common.bmg.txt old_waluigi_gc "GCN Waluigi Stadium"
.\bin\fart.exe Common.bmg.txt old_donkey_gc "GCN DK Mountain"
.\bin\fart.exe Common.bmg.txt old_falls_ds "DS Yoshi Falls"
.\bin\fart.exe Common.bmg.txt old_desert_ds "DS Desert Hills"
.\bin\fart.exe Common.bmg.txt old_garden_ds "DS Peach Gardens"
.\bin\fart.exe Common.bmg.txt old_town_ds "DS Delfino Square"
.\bin\fart.exe Common.bmg.txt old_mario_sfc "SNES Mario Circuit 3"
.\bin\fart.exe Common.bmg.txt old_obake_sfc "SNES Ghost Valley 2"
.\bin\fart.exe Common.bmg.txt old_mario_64 "N64 Mario Raceway"
.\bin\fart.exe Common.bmg.txt old_sherbet_64 "N64 Sherbet Land"
.\bin\fart.exe Common.bmg.txt old_koopa_64 "N64 Bowser's Castle"
.\bin\fart.exe Common.bmg.txt old_donkey_64 "N64 DK's Jungle Parkway"
.\bin\fart.exe Common.bmg.txt old_koopa_gba "GBA Bowser Castle 3"
.\bin\fart.exe Common.bmg.txt old_heyho_gba "GBA Shy Guy Beach"
.\bin\fart.exe Common.bmg.txt venice_battle "Delfino Pier"
.\bin\fart.exe Common.bmg.txt block_battle "Block Plaza"
.\bin\fart.exe Common.bmg.txt casino_battle "Chain Chomp Roulette"
.\bin\fart.exe Common.bmg.txt skate_battle "Funky Stadium"
.\bin\fart.exe Common.bmg.txt sand_battle "Thwomp Desert"
.\bin\fart.exe Common.bmg.txt old_CookieLand_gc "GCN Cookieland"
.\bin\fart.exe Common.bmg.txt old_House_ds "DS Twilight House"
.\bin\fart.exe Common.bmg.txt old_battle4_sfc "SNES Battle Course 4"
.\bin\fart.exe Common.bmg.txt old_battle3_gba "GBA Battle Course 3"
.\bin\fart.exe Common.bmg.txt old_matenro_64 "N64 Sky Scraper"
wbmgt encode Common.bmg.txt -o --quiet
del Common.bmg.txt
copy Common.bmg %MKW-PATH%/Scene/UI/
cd %MKW-PATH%/Scene/UI/
wszst patch *_?.szs --patch-bmg "replace=Common.bmg" --quiet