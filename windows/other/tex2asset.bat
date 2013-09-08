@echo off
setlocal enableextensions enabledelayedexpansion
for /R %%x in (*.tga) do (
	set name=%%~nx.texture.asset

	@echo mip_map_count =  0, > !name!
	@echo min_filter =  "linear", >> !name!
	@echo mag_filter =  "linear", >> !name!
	@echo wrap_s =  "repeat", >> !name!
	@echo wrap_t =  "repeat", >> !name!
	@echo color_image =  "%%~nx%%~xx", >> !name!
	@echo alpha_image =  "", >> !name!
	@echo color_quality =  100, >> !name!
	@echo alpha_quality =  100 >> !name!
)
pause
