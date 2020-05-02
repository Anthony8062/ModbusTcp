del *.obj /s /q
del *.iobj /s /q
del *.exp /s /q
del *.lib /s /q
del *.ilk /s /q
del *.rc /s /q
del *.res /s /q
del *.cache /s /q
del *.tlog /s /q
del *.lastbuildstate /s /q
del *.manifest /s /q
del *.log /s /q
del *.pch /s /q
md symbol
move *.pdb symbol/
move *.ipdb symbol/