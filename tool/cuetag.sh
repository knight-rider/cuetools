#! /bin/sh

# cuetag.sh - tag files based on cue/toc file information
# uses cueprint output
# usage: cuetag.sh <cuefile|tocfile> [file]...

CUEPRINT=`which cueprint`
CUE_I=""

# print usage instructions
usage()
{
	echo
	echo "usage: cuetag.sh <cuefile|tocfile> [file|tagfield]..."
	echo "       cuetag.sh <cuefile|tocfile>"
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
	echo "ogg/flac: ALBUM ALBUMARTIST ARTIST CONTACT COPYRIGHT DATE DESCRIPTION DISCNUMBER GENRE ISRC LICENSE LOCATION ORGANIZATION PERFORMER TITLE TRACKNUMBER TRACKTOTAL VERSION"
	echo "mp3: TITLE ALBUM ARTIST YEAR COMMENT GENRE TRACKNUMBER"
	echo
	echo "cuetag.sh uses cueprint, which must be in your path"
}

# Vorbis Comments
# for FLAC and Ogg Vorbis files
vorbis()
{
	trackno=$1; shift
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
	# see http://www.xiph.org/ogg/vorbis/doc/v-comment.html
	# TRACKTOTAL is not in the Xiph recommendation, but is in common use

	[ -n "$fields" ] ||
	fields='ALBUM ALBUMARTIST ARTIST CONTACT COPYRIGHT DATE DESCRIPTION DISCNUMBER GENRE ISRC LICENSE LOCATION ORGANIZATION PERFORMER TITLE TRACKNUMBER TRACKTOTAL VERSION'

	# fields' corresponding cueprint conversion characters
	# separate alternates with a space

	ALBUM='%T'
	ALBUMARTIST='%C %P'
	ARTIST='%c %p'
	CONTACT=''
	COPYRIGHT=''
	DATE='%Y'
	DESCRIPTION='%m'
	DISCNUMBER='%D'
	GENRE='%g'
	ISRC='%i %u'
	LICENSE=''
	LOCATION=''
	ORGANIZATION=''
	PERFORMER='%p'
	TITLE='%t'
	TRACKNUMBER='%02n'
	TRACKTOTAL='%02N'
	VERSION=''

	(for field in $fields; do
		 case "$field" in
		 (*=*) echo "$field";;
		 (*)
			 value=""
			 for conv in $(eval echo \$$field); do
				 value=$($CUEPRINT -n $trackno -t "$conv\n" "$CUE_I")

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
		echo "error: not found '(m)id3v2'."
		exit 1
	fi

	trackno=$1; shift
	file="$1"; shift
	fields="$@"

	# space separated list of ID3 v1.1 tags
	# see http://id3lib.sourceforge.net/id3/idev1.html

	[ -n "$fields" ] ||
	fields="TITLE ALBUM ARTIST YEAR COMMENT GENRE TRACKNUMBER"

	# fields' corresponding cueprint conversion characters
	# separate alternates with a space

	TITLE='%t'
	ALBUM='%T'
	ARTIST='%p'
	YEAR='%Y'
	COMMENT='%c'
	GENRE='%g'
	TRACKNUMBER='%n'

	for field in $fields; do
		case "$field" in
		*=*) value="${field#*=}";;
		*)
			value=""
			for conv in $(eval echo \$$field); do
				value=$($CUEPRINT -n $trackno -t "$conv\n" "$CUE_I")

				if [ -n "$value" ]; then
					break
				fi
			done
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
			ARTIST)
				$MP3TAG -a "$value" "$file"
				;;
			YEAR)
				$MP3TAG -y "$value" "$file"
				;;
			COMMENT)
				$MP3TAG -c "$value" "$file"
				;;
			GENRE)
				$MP3TAG -g "$value" "$file"
				;;
			TRACKNUMBER)
				$MP3TAG -T "$value" "$file"
				;;
			esac
		fi
	done
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
	trackno=0

	if [[ -z $@ ]]; then
		echo "WARNING: no filename given, will use name(s) in CUE/TOC sheet"
		while [ $trackno -lt $ntrack ]; do
			trackno=$(($trackno + 1))
			files[$trackno]=`$CUEPRINT -n $trackno -t '%f' "$CUE_I"`
		done
		set "${files[@]}"
		trackno=0
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

	CDNO=`$CUEPRINT -d '%D' "$CUE_I"|sed 's/.*/ CD&/'`
	CUE_O=`echo $(dirname "${FILE[1]}")/$($CUEPRINT -d "%P - %Y - %T$CDNO.cue" "$CUE_I"|sed 's.[/|\].-.g')|sed 's|^\./||'`
	[[ -f $CUE_O ]] && mv -v "$CUE_O" "BAK $CUE_O"
	[[ $CUE_I = $CUE_O ]] && CUE_I="BAK $CUE_O"
	$CUEPRINT -d 'PERFORMER \"%P\"\nTITLE \"%T\"\n' "$CUE_I" > "$CUE_O"
	grep -ve PERFORMER -ve TITLE -ve FILE -ve "INDEX 00" -ve "INDEX 01" "$CUE_I" | sed 's/^\xEF\xBB\xBF//;s/\r//' >> "$CUE_O"
	echo "Creating multi-file CUE sheet: $CUE_O"

	for file in "${FILE[@]}"; do
		IDX0=`cuebreakpoints -l "$CUE_I"|grep "^$trackno\s"|sed "s/.*\s//;s/./    INDEX 00 &/;s/\./:/"`
		trackno=$(($trackno + 1))
		LBL=`echo $(dirname "$file")/$($CUEPRINT -n $trackno -t '%02n %p - %t' "$CUE_I"|sed 's.[/|\].-.g')|sed 's|^\./||'`
		TYPE="WAVE"

		case $file in
		*.[Ff][Ll][Aa][Cc])
			vorbis $trackno "$file" $FIELDS
			LBL="$LBL.flac"
			;;
		*.[Oo][Gg][Gg])
			vorbis $trackno "$file" $FIELDS
			LBL="$LBL.ogg"
			;;
		*.[Mm][Pp]3)
			id3 $trackno "$file" $FIELDS
			LBL="$LBL.mp3"
			TYPE="MP3"
			;;
		*.[Tt][Xx][Tt])
			vorbis $trackno "$file"
			LBL="$LBL.txt"
			;;
		esac
		[[ $file != $LBL ]] && mv -v "$file" "$LBL"

		TFILE=`echo "FILE \"$LBL\" $TYPE\n"|sed 's|".*/|"|;s/\&/\\\&/g'`
		TITLE=`$CUEPRINT -n $trackno -t '%t' "$CUE_I"|sed 's/\&/\\\&/g'`
		PERFORMER=`$CUEPRINT -n $trackno -t '%p' "$CUE_I"|sed 's/\&/\\\&/g'`
		TRKNO=`echo $trackno|sed 's/^.$/0&/'`
		if [ "$IDX0" ]; then
			sed "s|^.*TRACK.*$TRKNO.*|\n&\n    TITLE \"$TITLE\"\n    PERFORMER \"$PERFORMER\"\n$IDX0\n$TFILE    INDEX 01 00:00:00|" "$CUE_O" > cue.out
		else
			sed "s|^.*TRACK.*$TRKNO.*|\n$TFILE&\n    TITLE \"$TITLE\"\n    PERFORMER \"$PERFORMER\"\n    INDEX 01 00:00:00|" "$CUE_O" > cue.out
		fi
		mv cue.out "$CUE_O"
	done
}

main "$@"
