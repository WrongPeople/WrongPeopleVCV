#!/bin/bash


cd "$(dirname "${BASH_SOURCE[0]}")/.."
EXPORT="python $(realpath tools/svg-objects-export.py)"
cd res/ComponentLibrary


rm -f Button_*.svg
$EXPORT -D --pattern '^Button_' --prefix '' -t plain-svg -X"-j" Button.svg

rm -f SwitchHorizontal_*.svg
$EXPORT -D --pattern '^SwitchHorizontal_' --prefix '' -t plain-svg -X"-j" SwitchHorizontal.svg

rm -f SwitchVertical_*.svg
$EXPORT -D --pattern '^SwitchVertical_' --prefix '' -t plain-svg -X"-j" SwitchVertical.svg
