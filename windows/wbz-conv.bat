@echo off
wszst normalize wbz-files/*.wbz --szs --overwrite
wszst normalize wbz-files/*.bz --szs --overwrite
del .\wbz-files\*.bz
move .\wbz-files\*.szs .\szs-files\
exit