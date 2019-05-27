#! /bin/sh

# cuetag.sh - tag files based on cue/toc file information
# uses cueprint output
# usage: cuetag.sh <cuefile|tocfile> [file]...

# https://wiki.hydrogenaud.io/index.php?title=Tag_Mapping

CUEPRINT=`which cueprint`
CUE_I=""

# print usage instructions
usage()
{
	echo "usage: cuetag.sh <cuefile|tocfile> [file|tag|tag=value]..."
	echo
	echo "cuetag.sh adds tags to files based on CUE/TOC information"
	echo
	echo "Supported formats (format extension, format name, tagging utility):"
	echo "ogg, Ogg Vorbis, vorbiscomment"
	echo "flac, FLAC, metaflac"
	echo "mp3, MP3, mp3info"
	echo "txt, Vorbis Comment Text File, tee"
	echo
	echo "Supported tag fields:"
	echo "ogg/flac: ALBUM ALBUMARTIST ARTIST COMPOSER DATE DESCRIPTION DISCNUMBER GENRE ISRC PERFORMER TITLE TRACKNUMBER TRACKTOTAL"
	echo "mp3:      ALBUM ALBUMARTIST ARTIST COMPOSER DATE DESCRIPTION DISCNUMBER GENRE ISRC TITLE TRACKNUMBER"
	echo
	echo "cuetag.sh uses cueprint, which must be in your path"
}

# Vorbis Comments
# for FLAC and Ogg Vorbis files
vorbis()
{
	TRACKNUMBER=$1; shift
	file="$1"; shift
	fields="$@"

	# FLAC tagging
	#  --remove-all-tags overwrites existing comments
	METAFLAC="metaflac --remove-all-tags --import-tags-from=-"

	# Ogg Vorbis tagging
	# -w overwrites existing comments
	# -a appends to existing comments
	VORBISCOMMENT="vorbiscomment -w -c -"

	# VC text file format
	# TODO: this also outputs to stdout
	TXTFILE="tee"

	case "$file" in
	*.[Ff][Ll][Aa][Cc])
		VORBISTAG=$METAFLAC
		;;
	*.[Oo][Gg][Gg])
		VORBISTAG=$VORBISCOMMENT
		;;
	*.[Tt][Xx][Tt])
		VORBISTAG=$TXTFILE
		;;
	esac

	# space separated list of recommended standard field names
	[ -n "$fields" ] ||
	fields='ALBUM ALBUMARTIST ARTIST COMPOSER DATE DESCRIPTION DISCNUMBER GENRE ISRC PERFORMER TITLE TRACKNUMBER TRACKTOTAL'

	(for field in $fields; do
		case "$field" in
		(*=*) echo "$field";;
		(*)
			values=$(eval echo \$$field)
			if [ "${values:0:1}" != '%' ]; then
				echo "$field=$values"
				continue
			fi
			for conv in $values; do
				value=$($CUEPRINT -n $TRACKNUMBER -t "$conv\n" "$CUE_I")
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
	MP3TAG=$(which mid3v2) \
		|| MP3TAG=$(which id3v2)
	if [ -z "${MP3TAG}" ]; then
		echo "ERROR: not found '(m)id3v2'."
		exit 1
	fi

	TRACKNUMBER=$1; shift
	file="$1"; shift
	fields="$@"

	# space separated list of ID3 v1.1 tags
	[ -n "$fields" ] ||
	fields="ALBUM ALBUMARTIST ARTIST COMPOSER DATE DESCRIPTION DISCNUMBER GENRE ISRC TITLE TRACKNUMBER"

	for field in $fields; do
		case "$field" in
		*=*) value="${field#*=}";;
		*)
			values=$(eval echo \$$field)
			if [ "${values:0:1}" != '%' ]; then
				value=$values
			else
				for conv in $values; do
					value=$($CUEPRINT -n $TRACKNUMBER -t "$conv\n" "$CUE_I")
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
			TRACKNUMBER)
				$MP3TAG -T "$value" "$file"
				;;
			esac
		fi
	done
}

# Wikipedia Style Capitalization Rules
cap()
{
	if [[ "$@" =~ [A-Z][A-Z] ]]; then
		# only trim all-caps sentence
		echo $@|awk '{$1=$1}1'
	else
		echo $@|awk '{$1=$1}1'|sed "s/\b./\u\0/g;
		s/\(\w\)'\(.\)/\1'\L\2/g;
		s/\s\(A\|An\|The\)\s/\L&/g;
		s/\s\(And\|But\|Or\|Nor\)\s/\L&/g;
		s/\s\(As\|At\|By\|In\|On\|Upon\)\s/\L&/g;
		s/\s\(For\|From\|Of\|Into\|To\|With\)\s/\L&/g;"
	fi
}

main()
{
	if [[ $# -lt 1 || -z $CUEPRINT ]]; then
		usage
		exit
	fi

	CUE_I=$1
	shift

	ntrack=$($CUEPRINT -d '%N' "$CUE_I")
	TRACKNUMBER=0

	if [[ -z $@ ]]; then
		echo "WARNING: no filename given, will use name(s) in CUE/TOC sheet"
		while [ $TRACKNUMBER -lt $ntrack ]; do
			TRACKNUMBER=$(($TRACKNUMBER + 1))
			files[$TRACKNUMBER]=`$CUEPRINT -n $TRACKNUMBER -t '%f' "$CUE_I"`
		done
		set "${files[@]}"
		TRACKNUMBER=0
	fi

	NFILE=0 FIELDS=
	for arg in "$@"; do
		case "$arg" in
		*.[Cc][Uu][Ee] | *.[Tt][Oo][Cc]) echo "ERROR: multiple CUE/TOC input"; exit 1;;
		*.[Ff][Ll][Aa][Cc] | *.[Oo][Gg][Gg] | *.[Mm][Pp]3 | *.[Tt][Xx][Tt])
			[[ ! -f $arg ]] && continue
			NFILE=$(($NFILE + 1))
			FILE[$NFILE]=$arg
			;;
		*) FIELDS="$FIELDS $arg";;
		esac
	done

	if [ $NFILE -ne $ntrack ]; then
		echo "ERROR: number of files ($NFILE) does not match number of tracks ($ntrack)"
		exit 1
	fi

	# fields' corresponding cueprint conversion characters
	# separate alternates with a space
	COMPOSER='%c'
	DESCRIPTION='%m'
	GENRE='%g'
	ISRC='%i %u'
	PERFORMER='%p'
	TRACKTOTAL='%02N'
	ALBUM=`cap $($CUEPRINT -d '%T' "$CUE_I")`
	ALBUMARTIST=`cap $($CUEPRINT -d '%C %P' "$CUE_I")`
	DATE=`$CUEPRINT -d '%Y' "$CUE_I"`
	DISCNUMBER=`$CUEPRINT -d '%D' "$CUE_I"`

	CUE_O=`echo $(dirname "${FILE[1]}")/$(echo "$ALBUMARTIST - $DATE - $ALBUM CD$DISCNUMBER.cue"|sed 's/ CD\./\./;s.[/|\].-.g')|sed 's|^\./||'`
	echo "Creating multi-file CUE sheet: $CUE_O"
	BAK="`dirname "$CUE_O"`/BAK `basename "$CUE_O"`"
	[[ -f $CUE_O ]] && mv -v "$CUE_O" "$BAK"
	[[ $CUE_I = $CUE_O ]] && CUE_I=$BAK

	echo PERFORMER \"$ALBUMARTIST\" > "$CUE_O"
	echo TITLE \"$ALBUM\" >> "$CUE_O"
	grep -ve '^\s*$' -ve PERFORMER -ve TITLE -ve FILE -ve "INDEX 00" -ve "INDEX 01" "$CUE_I"|sed 's/^\xEF\xBB\xBF//;s/\r//'>>"$CUE_O"

	for file in "${FILE[@]}"; do
		IDX0=`cuebreakpoints -l "$CUE_I"|grep "^$TRACKNUMBER\s"|sed "s/.*\s//;s/./    INDEX 00 &/;s/\./:/"`
		TRACKNUMBER=`echo $(expr $TRACKNUMBER + 1)|sed 's/^.$/0&/'`
		ARTIST=`cap $($CUEPRINT -n $TRACKNUMBER -t '%c %p' "$CUE_I")`
		TITLE=`cap $($CUEPRINT -n $TRACKNUMBER -t '%t' "$CUE_I")`
		LBL=`echo $(dirname "$file")/$(echo $TRACKNUMBER $ARTIST - $TITLE|sed 's.[/|\].-.g')|sed 's|^\./||'`
		TYPE="WAVE"

		case $file in
		*.[Ff][Ll][Aa][Cc])
			vorbis $TRACKNUMBER "$file" $FIELDS
			LBL="$LBL.flac"
			;;
		*.[Oo][Gg][Gg])
			vorbis $TRACKNUMBER "$file" $FIELDS
			LBL="$LBL.ogg"
			;;
		*.[Mm][Pp]3)
			id3 $TRACKNUMBER "$file" $FIELDS
			LBL="$LBL.mp3"
			TYPE="MP3"
			;;
		*.[Tt][Xx][Tt])
			vorbis $TRACKNUMBER "$file"
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
	done
}

main "$@"
