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

#------------------------------------------------------------------------------
# check for all binaries
if [ ! -d ./${BUILDS_PATH}/${VST_PATH}/${VST_NAME} ]; then
   echo "error: Couldn't find VST, please build binary first" >&2; exit 1
fi
echo "found VST."

pushd ./${BUILDS_PATH}/${VST_PATH} > /dev/null
if [ -f "${VST_NAME}.zip" ]; then
   rm  ${VST_NAME}.zip
fi
xcrun stapler staple ${VST_NAME}
popd > /dev/null



if [ ! -d ./${BUILDS_PATH}/${VST3_PATH}/${VST3_NAME} ]; then
   echo "error: Couldn't find VST3, please build binary first" >&2; exit 1
fi
echo "found VST3."

pushd ./${BUILDS_PATH}/${VST3_PATH} > /dev/null
if [ -f "${VST3_NAME}.zip" ]; then
   rm  ${VST3_NAME}.zip
fi
xcrun stapler staple ${VST3_NAME}
popd > /dev/null



if [ ! -d ./${BUILDS_PATH}/${AU_PATH}/${AU_NAME} ]; then
   echo "error: Couldn't find AU, please build binary first" >&2; exit 1
fi
echo "found AU."

pushd ./${BUILDS_PATH}/${AU_PATH} > /dev/null
if [ -f "${AU_NAME}.zip" ]; then
   rm  ${AU_NAME}.zip
fi
xcrun stapler staple ${AU_NAME}
popd > /dev/null


# if [ ! -d ./${BUILDS_PATH}/${AAX_ORIG_PATH}/${AAX_NAME} ]; then
#    echo "error: Couldn't find AAX, please build binary first" >&2; exit 1
# fi
# echo -n "found AAX. signing..."

# # get password from keychain.
# PASSWORD="$(security 2>&1 >/dev/null find-generic-password -ga iLok_mrest | cut -d " " -f 2 | sed  's/"//g' | sed 's/[^-A-Za-z0-9_]/\\&/g')"
# INFILE="./${BUILDS_PATH}/${AAX64_ORIG_PATH}/${AAX_NAME}"
# OUTFILE="./${BUILDS_PATH}/${AAX64_SIGNED_PATH}/${AAX_NAME}"

# # make sure to remove --installedbinaries as soon as PACE announces fix for 10.11.4 signing problems
# SIGN="/Applications/PACEAntiPiracy/Eden/Fusion/Current/bin/wraptool sign \
# 																	--account m.rest \
# 																	--wcguid DEE07770-E13F-11E5-A208-005056875CC3 \
# 																	--signid E-RM\ Erfindungsbuero \
# 																	--in $INFILE \
# 																	--out $OUTFILE \
# 																	--password $PASSWORD "
# eval $SIGN

# if [ $? -eq 0 ]
# then
# 	echo "ready."
# else
# 	echo "error: signing failed" >&2; exit 1
# fi


#------------------------------------------------------------------------------
# read release number com cli
read -p "Enter first digit of release: " n1
re='^[0-9]+$'
if ! [[ $n1 =~ $re ]] ; then
   echo "error: Not a number" >&2; exit 1
fi

read -p "Enter second digit of release: " n2
re='^[0-9]+$'
if ! [[ $n2 =~ $re ]] ; then
   echo "error: Not a number" >&2; exit 1
fi

read -p "Enter third digit of release: " n3
re='^[0-9]+$'
if ! [[ $n3 =~ $re ]] ; then
   echo "error: Not a number" >&2; exit 1
fi

VERSION=${RELEASE_ZIP_PRENAME}-${n1}_${n2}_${n3}

#------------------------------------------------------------------------------
# check if release already exists, otherwise create directory
if [ -d "${VERSION}" ]; then
    echo "error: Release folder already exists. Please delete manually to re-package binaries." >&2; exit 1
fi
if [ -f "${VERSION}.zip" ]; then
    echo "error: Release ZIP already exists. Please delete manually to re-package binaries." >&2; exit 1
fi


#------------------------------------------------------------------------------
# create subfolders and copy READMEs
echo "create (sub-)folders for release."
mkdir -p $VERSION/VST
cp READMEs/INSTALL_VST.txt $VERSION/VST

mkdir -p $VERSION/VST3
cp READMEs/INSTALL_VST3.txt $VERSION/VST3

mkdir -p $VERSION/AU
cp READMEs/INSTALL_AU.txt $VERSION/AU

#mkdir -p $VERSION/AAX
#cp READMEs/INSTALL_AAX.txt $VERSION/AAX


#------------------------------------------------------------------------------
#pushd ./${BUILDS_PATH}/${AAX_SIGNED_PATH} > /dev/null
#echo "packing and moving AAX into release tree."
#zip -qr --symlinks ${AAX_NAME}.zip ./${AAX_NAME}/ -x "*.DS_Store"
#popd > /dev/null
#mv ./${BUILDS_PATH}/${AAX_SIGNED_PATH}/${AAX_NAME}.zip $VERSION/AAX/

pushd ./${BUILDS_PATH}/${VST_PATH} > /dev/null
echo "packing and moving VST into release tree."
zip -qr --symlinks ${VST_NAME}.zip ./${VST_NAME}/ -x "*.DS_Store"
popd > /dev/null
mv ./${BUILDS_PATH}/${VST_PATH}/${VST_NAME}.zip $VERSION/VST/

pushd ./${BUILDS_PATH}/${VST3_PATH} > /dev/null
echo "packing and moving VST3 into release tree."
zip -qr --symlinks ${VST3_NAME}.zip ./${VST3_NAME}/ -x "*.DS_Store"
popd > /dev/null
mv ./${BUILDS_PATH}/${VST3_PATH}/${VST3_NAME}.zip $VERSION/VST3/

pushd ./${BUILDS_PATH}/${AU_PATH} > /dev/null
echo "packing and moving AU into release tree."
zip -qr --symlinks ${AU_NAME}.zip ./${AU_NAME}/ -x "*.DS_Store"
popd > /dev/null
mv ./${BUILDS_PATH}/${AU_PATH}/${AU_NAME}.zip $VERSION/AU/


#------------------------------------------------------------------------------
echo "packing all up and saving as ${VERSION}.zip"
zip -qr ${VERSION}.zip ./${VERSION}/ -x "*.DS_Store"
rm -r ./${VERSION}/


