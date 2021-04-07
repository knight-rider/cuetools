#! /bin/sh

# cuetag.sh - tag files based on cue/toc file information
# uses cueprint output
# usage: cuetag.sh <cuefile|tocfile> [file]...

# https://wiki.hydrogenaud.io/index.php?title=Tag_Mapping

# print usage instructions
usage()
{
	echo "usage: cuetag.sh <cuefile|tocfile> [file|tag|tag=value]..."
	echo
	echo "cuetag.sh adds tags to files based on CUE/TOC information"
	echo "          also creates M3U & (non-compliant) multi-file CUE sheets"
	echo
	echo "Supported formats (format extension, format name, tagging utility):"
	echo "ogg, Ogg Vorbis, vorbiscomment"
	echo "flac, FLAC, metaflac"
	echo "mp3, MP3, (m)id3v2"
	echo "txt, Vorbis Comment Text File, tee"
	echo
	echo "Supported tag fields:"
	echo "ALBUM ALBUMARTIST ARTIST COMPOSER DATE DESCRIPTION DISCNUMBER GENRE ISRC PERFORMER TITLE TRACKNUMBER TRACKTOTAL"
	exit
}

# Vorbis Comments
# for FLAC and Ogg Vorbis files
vorbis()
{
	[ `which metaflac` ] || exit
	[ `which vorbiscomment` ] || exit

	case "$file" in
	*.[Ff][Ll][Aa][Cc])
		# FLAC tagging
		#  --remove-all-tags overwrites existing comments
		VORBISTAG="metaflac --remove-all-tags --import-tags-from=-"
		;;
	*.[Oo][Gg][Gg])
		# Ogg Vorbis tagging
		# -w overwrites existing comments
		# -a appends to existing comments
		VORBISTAG="vorbiscomment -w -c -"
		;;
	*.[Tt][Xx][Tt])
		VORBISTAG="tee"
		;;
	esac

	(for field in $FIELDS; do
		case "$field" in
		(*=*) echo "$field";;
		(*)
			values=$(eval echo \$$field)
			if [ "${values:0:1}" != '%' ]; then
				echo "$field=$values"
				continue
			fi
			for conv in $values; do
				value=$(cueprint -n $TRACKNUMBER -t "$conv\n" "$CUE_I")
				if [ -n "$value" ]; then
					echo "$field=$value"
					break
				fi
			done
			;;
		esac
	done) | $VORBISTAG "$file"
}

id3()
{
	MP3TAG=$(which mid3v2-3 mid3v2 id3v2 2>/dev/null|head -n 1)
	if [ -z "${MP3TAG}" ]; then
		echo "ERROR: not found '(m)id3v2'."
		exit
	fi

	for field in $FIELDS; do
		case "$field" in
		*=*) value="${field#*=}";;
		*)
			values=$(eval echo \$$field)
			if [ "${values:0:1}" != '%' ]; then
				value=$values
			else
				for conv in $values; do
					value=$(cueprint -n $TRACKNUMBER -t "$conv\n" "$CUE_I")
					[ -n "$value" ] && break
				done
			fi
			;;
		esac

		if [ -n "$value" ]; then
			case $field in
			TITLE)
				$MP3TAG -t "$value" "$file"
				;;
			ALBUM)
				$MP3TAG -A "$value" "$file"
				;;
			ALBUMARTIST)
				$MP3TAG --TPE2 "$value" "$file"
				;;
			ARTIST)
				$MP3TAG -a "$value" "$file"
				;;
			COMPOSER)
				$MP3TAG --TCOM "$value" "$file"
				;;
			DATE)
				$MP3TAG -y "$value" "$file"
				;;
			DESCRIPTION)
				$MP3TAG --TIT3 "$value" "$file"
				;;
			DISCNUMBER)
				$MP3TAG --TPOS "$value" "$file"
				;;
			GENRE)
				$MP3TAG -g "$value" "$file"
				;;
			ISRC)
				$MP3TAG --TSRC "$value" "$file"
				;;
			PERFORMER)
				$MP3TAG --IPLS "$value" "$file"
				;;
			TRACKNUMBER)
				$MP3TAG -T "$value" "$file"
				;;
			TRACKTOTAL)
				$MP3TAG --TRCK "$TRACKNUMBER/$value" "$file"
				;;
			esac
		fi
	done
}

# Wikipedia Style Capitalization Rules
cap()
{
	echo $@|awk '{$1=$1}1'|sed "s/\b./\u\0/g;
	s/\b'\(D\|Ll\|M\|Re\|S\|T\|Ve\)\b/\L&/g;
	s/\s\(A\|An\|The\)\s/\L&/g;
	s/\s\(And\|But\|Or\|Nor\)\s/\L&/g;
	s/\s\(As\|At\|By\|In\|On\|Upon\)\s/\L&/g;
	s/\s\(For\|From\|Of\|Into\|To\|With\)\s/\L&/g;"
}

main()
{
	[ `which cueprint` ] || exit
	[ `which cuebreakpoints` ] || exit

	[ $# -lt 1 ] && usage
	[ ! -r $1 ] && usage
	case "$1" in
	*.[Cc][Uu][Ee] | *.[Tt][Oo][Cc]) CUE_I=$1;;
	*) usage;;
	esac

	shift
	NTRACK=$(cueprint -d '%N' "$CUE_I")
	TRACKNUMBER=0

	if [[ -z $@ ]]; then
		echo "WARNING: no filename given, will use name(s) in CUE/TOC sheet"
		while [ $TRACKNUMBER -lt $NTRACK ]; do
			TRACKNUMBER=$(($TRACKNUMBER + 1))
			files[$TRACKNUMBER]=`cueprint -n $TRACKNUMBER -t '%f' "$CUE_I"`
		done
		set "${files[@]}"
		TRACKNUMBER=0
	fi

	NFILE=0 FIELDS=
	for arg in "$@"; do
		case "$arg" in
		*.[Cc][Uu][Ee] | *.[Tt][Oo][Cc]) echo "ERROR: multiple CUE/TOC input"; exit;;
		*.[Ff][Ll][Aa][Cc] | *.[Oo][Gg][Gg] | *.[Mm][Pp]3 | *.[Tt][Xx][Tt])
			[[ ! -f $arg ]] && continue
			NFILE=$(($NFILE + 1))
			FILE[$NFILE]=$arg
			;;
		*) FIELDS="$FIELDS $arg";;
		esac
	done

	if [ $NFILE -ne $NTRACK ]; then
		echo "ERROR: number of files ($NFILE) does not match number of tracks ($NTRACK)"
		exit
	fi

	# Space separated list of Vorbis tags
	[ -n "$FIELDS" ] ||
	FIELDS="ALBUM ALBUMARTIST ARTIST COMPOSER DATE DESCRIPTION DISCNUMBER GENRE ISRC PERFORMER TITLE TRACKNUMBER TRACKTOTAL"

	# fields' corresponding cueprint conversion characters
	# separate alternates with a space
	ALBUM=`cap $(cueprint -d '%T' "$CUE_I")`
	ALBUMARTIST=`cap $(cueprint -d '%C %P' "$CUE_I")`
	DATE=`cueprint -d '%Y' "$CUE_I"`
	DISCNUMBER=`cueprint -d '%D' "$CUE_I"`
	DESCRIPTION='%m'
	GENRE='%g'
	ISRC='%i %u'
	TRACKTOTAL='%02N'

	CUE_O=`echo $(dirname "${FILE[1]}")/$(echo "$ALBUMARTIST - $DATE - $ALBUM CD$DISCNUMBER.cue"|sed 's/ CD\./\./;s.[/|\].-.g')|sed 's|^\./||'`
	echo "Creating multi-file CUE sheet: $CUE_O"
	BAK="`dirname "$CUE_O"`/BAK `basename "$CUE_O"`"
	[[ -f $CUE_O ]] && mv -v "$CUE_O" "$BAK"
	[[ $CUE_I = $CUE_O ]] && CUE_I=$BAK

	M3U_O=`echo "$CUE_O"|sed 's/cue$/m3u/'`
	echo "Creating M3U sheet: $M3U_O"
	echo "#EXTM3U" > $M3U_O

	echo PERFORMER \"$ALBUMARTIST\" > $CUE_O
	echo TITLE \"$ALBUM\" >> $CUE_O
	grep -ve '^\s*$' -ve PERFORMER -ve TITLE -ve FILE -ve "INDEX 00" -ve "INDEX 01" "$CUE_I"|sed 's/^\xEF\xBB\xBF//;s/\r//'>>$CUE_O

	for file in "${FILE[@]}"; do
		IDX0=`cuebreakpoints -l "$CUE_I"|grep "^$TRACKNUMBER\s"|sed "s/.*\s//;s/./    INDEX 00 &/;s/\./:/"`
		TRACKNUMBER=`echo $(expr $TRACKNUMBER + 1)|sed 's/^.$/0&/'`
		ARTIST=`cap $(cueprint -n $TRACKNUMBER -t '%c %p' "$CUE_I")`
		COMPOSER=`cap $(cueprint -n $TRACKNUMBER -t '%c' "$CUE_I")`
		PERFORMER=`cap $(cueprint -n $TRACKNUMBER -t '%p' "$CUE_I")`
		TITLE=`cap $(cueprint -n $TRACKNUMBER -t '%t' "$CUE_I")`
		LBL=`echo $(dirname "$file")/$(echo $TRACKNUMBER $ARTIST - $TITLE|sed 's.[/|\].-.g')|sed 's|^\./||'`
		TYPE="WAVE"

		DURATION=0
		case $file in
		*.[Ff][Ll][Aa][Cc])
			vorbis
			LBL="$LBL.flac"
			DURATION=`metaflac --show-total-samples --show-sample-rate "$file"| tr '\n' ' '|awk '{print $1/$2}'`
			;;
		*.[Oo][Gg][Gg])
			vorbis
			LBL="$LBL.ogg"
			;;
		*.[Mm][Pp]3)
			id3
			LBL="$LBL.mp3"
			TYPE="MP3"
			[ `which mp3info` ] && DURATION=`mp3info -p "%S" "$file"`
			;;
		*.[Tt][Xx][Tt])
			vorbis
			LBL="$LBL.txt"
			;;
		esac
		[ "$file" != "$LBL" ] && mv -v "$file" "$LBL"

		TFILE=`echo "FILE \"$LBL\" $TYPE\n"|sed 's|".*/|"|;s/\&/\\\&/g'`
		TITLEARTIST=`echo "\n    TITLE \"$TITLE\"\n    PERFORMER \"$ARTIST\"\n"|sed 's/\&/\\\&/g'`
		if [ "$IDX0" ]; then
			sed "s|^.*TRACK.*$TRACKNUMBER.*|\n&$TITLEARTIST$IDX0\n$TFILE    INDEX 01 00:00:00|" "$CUE_O" > cue.out
		else
			sed "s|^.*TRACK.*$TRACKNUMBER.*|\n$TFILE&$TITLEARTIST    INDEX 01 00:00:00|" "$CUE_O" > cue.out
		fi
		mv cue.out "$CUE_O"

		echo "#EXTINF:$DURATION, $ARTIST - $TITLE" >> $M3U_O
		echo $LBL|sed 's|.*/||' >> $M3U_O
	done
}

main "$@"
