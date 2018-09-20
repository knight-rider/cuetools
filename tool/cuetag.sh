#! /bin/sh

# cuetag.sh - tag files based on cue/toc file information
# uses cueprint output
# usage: cuetag.sh <cuefile|tocfile> [file]...

CUEPRINT=`which cueprint`
cue_file=""

# print usage instructions
usage()
{
	echo
	echo "usage: cuetag.sh <cuefile|tocfile> [file]..."
	echo
	echo "cuetag.sh adds tags to files based on cue or toc information"
	echo
	echo "Supported formats (format extension, format name, tagging utility):"
	echo "ogg, Ogg Vorbis, vorbiscomment"
	echo "flac, FLAC, metaflac"
	echo "mp3, MP3, mp3info"
	echo "txt, Vorbis Comment Text File, tee"
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
				 value=$($CUEPRINT -n $trackno -t "$conv\n" "$cue_file")

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

	# space separated list of ID3 v1.1 tags
	# see http://id3lib.sourceforge.net/id3/idev1.html

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
				value=$($CUEPRINT -n $1 -t "$conv\n" "$cue_file")

				if [ -n "$value" ]; then
					break
				fi
			done
			;;
		esac

		if [ -n "$value" ]; then
			case $field in
			TITLE)
				$MP3TAG -t "$value" "$2"
				;;
			ALBUM)
				$MP3TAG -A "$value" "$2"
				;;
			ARTIST)
				$MP3TAG -a "$value" "$2"
				;;
			YEAR)
				$MP3TAG -y "$value" "$2"
				;;
			COMMENT)
				$MP3TAG -c "$value" "$2"
				;;
			GENRE)
				$MP3TAG -g "$value" "$2"
				;;
			TRACKNUMBER)
				$MP3TAG -T "$value" "$2"
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

	cue_file=$1
	shift

	ntrack=$($CUEPRINT -d '%N' "$cue_file")
	trackno=1

	NUM_FILES=0 FIELDS=
	for arg in "$@"; do
		case "$arg" in
		*.*) NUM_FILES=$(expr $NUM_FILES + 1);;
		*) FIELDS="$FIELDS $arg";;
		esac
	done

	if [ $NUM_FILES -ne $ntrack ]; then
		echo "warning: number of files does not match number of tracks"
	fi

	for file in "$@"; do
		LBL=`dirname "$file"`/`$CUEPRINT -n $trackno -t '%02n %p - %t' "$cue_file"|sed 's/\.$//'`

		case $file in
		*.[Ff][Ll][Aa][Cc])
			vorbis $trackno "$file" $FIELDS
			[[ $file != $LBL.flac ]] && `mv "$file" "$LBL.flac"`
			;;
		*.[Oo][Gg][Gg])
			vorbis $trackno "$file" $FIELDS
			[[ $file != $LBL.ogg ]] && `mv "$file" "$LBL.ogg"`
			;;
		*.[Mm][Pp]3)
			id3 $trackno "$file" $FIELDS
			[[ $file != $LBL.mp3 ]] && `mv "$file" "$LBL.mp3"`
			;;
		*.[Tt][Xx][Tt])
			vorbis $trackno "$file"
			[[ $file != $LBL.txt ]] && `mv "$file" "$LBL.txt"`
			;;
		*.*)
			echo "$file: uknown file type"
			;;
		esac
		trackno=$(($trackno + 1))
	done
}

main "$@"
