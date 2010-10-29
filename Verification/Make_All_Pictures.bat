@echo off

echo.
echo erasing User Guide scripted figures generated previously
erase ..\Manuals\FDS_5_User_Guide\SCRIPT_FIGURES\*.png

echo.
echo erasing Verification scripted figures generated previously
erase ..\Manuals\FDS_5_Verification_Guide\SCRIPT_FIGURES\*.png

cd Controls
smokeview -runscript activate_vents

cd ..\Detectors
smokeview -runscript beam_detector

cd ..\Fires
smokeview -runscript room_fire

cd ..\Flowfields
smokeview -runscript helium_2d
smokeview -runscript sawtooth
smokeview -runscript symmetry_test

cd ..\Miscellaneous
smokeview -runscript pyramid

cd ..\NS_Analytical_Solution
smokeview -runscript ns2d_64

cd ..\Pressure_Effects
smokeview -runscript pressure_boundary

cd ..\Scalar_Analytical_Solution
smokeview -runscript move_slug
smokeview -runscript move_slug_fl1

cd ..\Sprinklers_and_Sprays
smokeview -runscript cascade

cd ..\Visualization
smokeview -runscript objects_static
smokeview -runscript objects_dynamic


Rem generate Smokeview figures
cd ..\scripts
call Make_SMV_Pictures


