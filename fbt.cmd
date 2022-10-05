@echo off
SET cdir=%~dp0
pushd %cdir%
IF not exist flipperzero-firmware\applications_user\totp (bin\elevate -w -c mklink /D flipperzero-firmware\applications_user\totp ..\..\totp)
pushd flipperzero-firmware
fbt %*
popd
popd