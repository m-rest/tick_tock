#!/bin/bash
#
#
#
#
#------------------------------------------------------------------------------
WORKPATH="/Users/mrest/Development/E-RM/multiclock/releases/multiclock-plugin"
cd $WORKPATH

RELEASE_ZIP_PRENAME="multiclock-plugin-OSX-10_11up"

VST_NAME="multiclock.vst"
VST3_NAME="multiclock.vst3"
AU_NAME="multiclock.component"
AAX_NAME="multiclock.aaxplugin"

VST_PATH="VST"
VST3_PATH="VST3"
AU_PATH="AU"

AAX_ORIG_PATH="AAX"
AAX_SIGNED_PATH="AAX/signed"

BUILDS_PATH="justBuilt"

ASP_PASSWORD=""

#------------------------------------------------------------------------------
# check for all binaries
if [ ! -d ./${BUILDS_PATH}/${VST_PATH}/${VST_NAME} ]; then
   echo "error: Couldn't find VST, please build binary first" >&2; exit 1
fi
echo "found VST."

echo "signing.."
codesign -s "Developer ID Application: Erfindungsbuero GmbH " ./${BUILDS_PATH}/${VST_PATH}/${VST_NAME} --timestamp --force

echo "compress into zip."
pushd ./${BUILDS_PATH}/${VST_PATH} > /dev/null
zip -qr --symlinks ${VST_NAME}.zip ./${VST_NAME}/ -x "*.DS_Store"

echo "upload for notarizing"
xcrun altool --notarize-app --primary-bundle-id "com.erm.multiclock" --username "apple@e-rm.de" --password ${ASP_PASSWORD} --asc-provider "M2X35ZBUJ6" --file ${VST_NAME}.zip
popd > /dev/null



if [ ! -d ./${BUILDS_PATH}/${VST3_PATH}/${VST3_NAME} ]; then
   echo "error: Couldn't find VST3, please build binary first" >&2; exit 1
fi
echo "found VST3."

echo "signing.."
codesign -s "Developer ID Application: Erfindungsbuero GmbH " ./${BUILDS_PATH}/${VST3_PATH}/${VST3_NAME} --timestamp --force

echo "compress into zip."
pushd ./${BUILDS_PATH}/${VST3_PATH} > /dev/null
zip -qr --symlinks ${VST3_NAME}.zip ./${VST3_NAME}/ -x "*.DS_Store"

echo "upload for notarizing"
xcrun altool --notarize-app --primary-bundle-id "com.erm.multiclock" --username "apple@e-rm.de" --password ${ASP_PASSWORD} --asc-provider "M2X35ZBUJ6" --file ${VST3_NAME}.zip
popd > /dev/null




if [ ! -d ./${BUILDS_PATH}/${AU_PATH}/${AU_NAME} ]; then
   echo "error: Couldn't find AU, please build binary first" >&2; exit 1
fi
echo "found AU."

echo "signing.."
codesign -s "Developer ID Application: Erfindungsbuero GmbH " ./${BUILDS_PATH}/${AU_PATH}/${AU_NAME} --timestamp --force

echo "compress into zip."
pushd ./${BUILDS_PATH}/${AU_PATH} > /dev/null
zip -qr --symlinks ${AU_NAME}.zip ./${AU_NAME}/ -x "*.DS_Store"

echo "upload for notarizing"
xcrun altool --notarize-app --primary-bundle-id "com.erm.multiclock" --username "apple@e-rm.de" --password ${ASP_PASSWORD} --asc-provider "M2X35ZBUJ6" --file ${AU_NAME}.zip
popd > /dev/null


exit 0

