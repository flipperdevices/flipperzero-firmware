#!/bin/sh
### merge.sh attribution
# Based on version 23 of https://github.com/Zren/plasma-applet-lib/blob/master/package/translate/merge
# Adapted to Flipper Zero firmware applications


# Version: 23

# https://techbase.kde.org/Development/Tutorials/Localization/i18n_Build_Systems
# https://techbase.kde.org/Development/Tutorials/Localization/i18n_Build_Systems/Outside_KDE_repositories
# https://invent.kde.org/sysadmin/l10n-scripty/-/blob/master/extract-messages.sh

DIR=`cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd`
plasmoidName=`kreadconfig5 --file="$DIR/../application.fam" --group="Desktop Entry" --key="X-KDE-PluginInfo-Name"`
widgetName="Flipper Zero Firmware" # Strip namespace
website="https://github.com/flipperdevices/flipperzero-firmware/"
bugAddress="$website"
packageRoot=".." # Root of translatable sources
projectName="$widgetName" # project name

### Colors
# https://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
# https://stackoverflow.com/questions/911168/how-can-i-detect-if-my-shell-script-is-running-through-a-pipe
TC_Red='\033[31m'; TC_Orange='\033[33m';
TC_LightGray='\033[90m'; TC_LightRed='\033[91m'; TC_LightGreen='\033[92m'; TC_Yellow='\033[93m'; TC_LightBlue='\033[94m';
TC_Reset='\033[0m'; TC_Bold='\033[1m';
if [ ! -t 1 ]; then
	TC_Red=''; TC_Orange='';
	TC_LightGray=''; TC_LightRed=''; TC_LightGreen=''; TC_Yellow=''; TC_LightBlue='';
	TC_Bold=''; TC_Reset='';
fi
function echoTC() {
	text="$1"
	textColor="$2"
	echo -e "${textColor}${text}${TC_Reset}"
}
function echoGray { echoTC "$1" "$TC_LightGray"; }
function echoRed { echoTC "$1" "$TC_Red"; }
function echoGreen { echoTC "$1" "$TC_LightGreen"; }

#---
if [ -z "$(which xgettext)" ]; then
	echoRed "[translate/merge] Error: xgettext command not found. Need to install gettext"
	echoRed "[translate/merge] Running ${TC_Bold}'sudo apt install gettext'"
	sudo apt install gettext
	echoRed "[translate/merge] gettext installation should be finished. Going back to merging translations."
fi

#---
echoGray "[translate/merge] Extracting messages"
potArgs="--from-code=UTF-8 --width=200 --add-location=file"

# Note: xgettext v0.20.1 (Kubuntu 20.04) and below will attempt to translate Icon,
# so we need to specify Name.
# https://github.com/Zren/plasma-applet-lib/issues/1
# https://savannah.gnu.org/support/?108887
# find "${packageRoot}" -name '*.fam' | sort > "${DIR}/infiles.list"
# xgettext \
# 	${potArgs} \
# 	--files-from="${DIR}/infiles.list" \
# 	--language=Python \
# 	--keyword="name" --debug \
# 	-D "${packageRoot}" \
# 	-D "${DIR}" \
# 	-o "template.pot.new" \
# 	|| \
# 	{ echoRed "[translate/merge] error while calling xgettext. aborting."; exit 1; }

# sed -i 's/"Content-Type: text\/plain; charset=CHARSET\\n"/"Content-Type: text\/plain; charset=UTF-8\\n"/' "template.pot.new"

# See Ki18n's extract-messages.sh for a full example:
# https://invent.kde.org/sysadmin/l10n-scripty/-/blob/master/extract-messages.sh#L25
find "${packageRoot}/applications" -name '*.cpp' -o -name '*.h' -o -name '*.c' -o -name '*.qml' -o -name '*.js' | sort > "${DIR}/infiles.list"
touch ${DIR}/template.pot.new #gettext will complain otherwise
xgettext \
	${potArgs} \
	--files-from="${DIR}/infiles.list" \
	-C \
	--keyword="i18n" --keyword="i18np" \
	--package-name="${widgetName}" \
	--msgid-bugs-address="${bugAddress}" \
	-D "${packageRoot}" \
	-D "${DIR}" \
	--join-existing \
	-o "${DIR}/template.pot.new" \
	|| \
	{ echoRed "[translate/merge] error while calling xgettext. aborting."; exit 1; }

sed -i 's/# SOME DESCRIPTIVE TITLE./'"# Translation of ${widgetName} in LANGUAGE"'/' "template.pot.new"
sed -i 's/# Copyright (C) YEAR THE PACKAGE'"'"'S COPYRIGHT HOLDER/'"# Copyright (C) $(date +%Y)"'/' "template.pot.new"

if [ -f "template.pot" ]; then
	newPotDate=`grep "POT-Creation-Date:" template.pot.new | sed 's/.\{3\}$//'`
	oldPotDate=`grep "POT-Creation-Date:" template.pot | sed 's/.\{3\}$//'`
	sed -i 's/'"${newPotDate}"'/'"${oldPotDate}"'/' "template.pot.new"
	changes=`diff "template.pot" "template.pot.new"`
	if [ ! -z "$changes" ]; then
		# There's been changes
		sed -i 's/'"${oldPotDate}"'/'"${newPotDate}"'/' "template.pot.new"
		mv "template.pot.new" "template.pot"

		addedKeys=`echo "$changes" | grep "> msgid" | cut -c 9- | sort`
		removedKeys=`echo "$changes" | grep "< msgid" | cut -c 9- | sort`
		echo ""
		echoGreen "Added Keys:"
		echoGreen "$addedKeys"
		echo ""
		echoRed "Removed Keys:"
		echoRed "$removedKeys"
		echo ""

	else
		# No changes
		rm "template.pot.new"
	fi
else
	# template.pot didn't already exist
	mv "template.pot.new" "template.pot"
fi

potMessageCount=`expr $(grep -Pzo 'msgstr ""\n(\n|$)' "template.pot" | grep -c 'msgstr ""')`
echo "|  Locale  |  Lines  | % Done|" > "./Status.md"
echo "|----------|---------|-------|" >> "./Status.md"
entryFormat="| %-8s | %7s | %5s |"
templateLine=`perl -e "printf(\"$entryFormat\", \"Template\", \"${potMessageCount}\", \"\")"`
echo "$templateLine" >> "./Status.md"

rm "${DIR}/infiles.list"
echoGray "[translate/merge] Done extracting messages"

#---
echoGray "[translate/merge] Merging messages"
catalogs=`find . -name '*.po' | sort`
for cat in $catalogs; do
	echoGray "[translate/merge] Updating ${cat}"
	catLocale=`basename ${cat%.*}`

	widthArg=""
	catUsesGenerator=`grep "X-Generator:" "$cat"`
	if [ -z "$catUsesGenerator" ]; then
		widthArg="--width=400"
	fi

	compendiumArg=""
	if [ ! -z "$COMPENDIUM_DIR" ]; then
		langCode=`basename "${cat%.*}"`
		compendiumPath=`realpath "$COMPENDIUM_DIR/compendium-${langCode}.po"`
		if [ -f "$compendiumPath" ]; then
			echo "compendiumPath=$compendiumPath"
			compendiumArg="--compendium=$compendiumPath"
		fi
	fi

	cp "$cat" "$cat.new"
	sed -i 's/"Content-Type: text\/plain; charset=CHARSET\\n"/"Content-Type: text\/plain; charset=UTF-8\\n"/' "$cat.new"

	msgmerge \
		${widthArg} \
		--add-location=file \
		--no-fuzzy-matching \
		${compendiumArg} \
		-o "$cat.new" \
		"$cat.new" "${DIR}/template.pot"

	sed -i 's/# SOME DESCRIPTIVE TITLE./'"# Translation of ${widgetName} in ${catLocale}"'/' "$cat.new"
	sed -i 's/# Translation of '"${widgetName}"' in LANGUAGE/'"# Translation of ${widgetName} in ${catLocale}"'/' "$cat.new"
	sed -i 's/# Copyright (C) YEAR THE PACKAGE'"'"'S COPYRIGHT HOLDER/'"# Copyright (C) $(date +%Y)"'/' "$cat.new"

	poEmptyMessageCount=`expr $(grep -Pzo 'msgstr ""\n(\n|$)' "$cat.new" | grep -c 'msgstr ""')`
	poMessagesDoneCount=`expr $potMessageCount - $poEmptyMessageCount`
	poCompletion=`perl -e "printf(\"%d\", $poMessagesDoneCount * 100 / $potMessageCount)"`
	poLine=`perl -e "printf(\"$entryFormat\", \"$catLocale\", \"${poMessagesDoneCount}/${potMessageCount}\", \"${poCompletion}%\")"`
	echo "$poLine" >> "./Status.md"

	# mv "$cat" "$cat.old"
	mv "$cat.new" "$cat"
done
echoGray "[translate/merge] Done merging messages"

#---
# echoGray "[translate/merge] Updating .fam file"

# # Generate LINGUAS for msgfmt
# if [ -f "$DIR/LINGUAS" ]; then
# 	rm "$DIR/LINGUAS"
# fi
# touch "$DIR/LINGUAS"
# for cat in $catalogs; do
# 	catLocale=`basename ${cat%.*}`
# 	echo "${catLocale}" >> "$DIR/LINGUAS"
# done

# cp -f "$DIR/../application.fam" "$DIR/application.fam"
# sed -i '/^name\[/ d' "$DIR/application.fam"

# msgfmt \
# 	--desktop \
# 	--template="$DIR/application.fam" \
# 	-d "$DIR/" \
# 	-o "$DIR/new.fam"

# # Delete empty msgid messages that used the po header
# if [ ! -z "$(grep '^name=$' "$DIR/new.fam")" ]; then
# 	echo "[translate/merge] Name in application.fam is empty!"
# 	sed -i '/^name\[/ d' "$DIR/new.fam"
# fi

# # Place translations at the bottom of the fam file.
# translatedLines=`cat "$DIR/new.fam" | grep "]="`
# if [ ! -z "${translatedLines}" ]; then
# 	sed -i '/^Name\[/ d; /^GenericName\[/ d; /^Comment\[/ d; /^Keywords\[/ d' "$DIR/new.fam"
# 	if [ "$(tail -c 2 "$DIR/new.fam" | wc -l)" != "2" ]; then
# 		# Does not end with 2 empty lines, so add an empty line.
# 		echo "" >> "$DIR/new.fam"
# 	fi
# 	echo "${translatedLines}" >> "$DIR/new.fam"
# fi

# # Cleanup
# mv "$DIR/new.fam" "$DIR/../application.fam"
# rm "$DIR/template.fam"
# rm "$DIR/LINGUAS"

echoGreen "[translate/merge] Done merge script"
