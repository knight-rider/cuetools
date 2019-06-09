#!/bin/bash

# frontend for:			cuetools, shntool, mp3splt
# optional dependencies:	flac, mac, wavpack, ttaenc
# v1.3 sen

[[ -z `which shnsplit` ]] && exit 1
DIR=`pwd`
CUE=`ls *.cue`

while [ "$1" ]; do
	case $1 in
	-h | --help )
		echo "Usage: cuesplit [path|cuefile]"
		echo "	   The default path is the current directory."
		exit
		;;
	* )
		[[ -d $1 ]] && DIR=$1
		[[ -f $1 ]] && CUE=$1
		shift
	esac
done

echo -e "\

Directory: $DIR
________________________________________
"
cd "$DIR"
TYPE=`ls -t1`

case $TYPE in
*.ape*)
	mkdir split
	shnsplit -d split -f "$CUE" -o "flac flac -V --best -o %f -" *.ape -t "%n %p - %t"
	rm -f split/00*pregap*
	cuetag.sh "$CUE" split/*.flac
	exit
	;;

*.flac*)
	mkdir split
	shnsplit -d split -f "$CUE" -o "flac flac -V --best -o %f -" *.flac -t "%n %p - %t"
	rm -f split/00*pregap*
	cuetag.sh "$CUE" split/*.flac
	exit
	;;

*.mp3*)
	mp3splt -no "@n @p - @t (split)" -c "$CUE" *.mp3
	cuetag.sh "$CUE" *split\).mp3
	exit
	;;

*.ogg*)
	mp3splt -no "@n @p - @t (split)" -c "$CUE" *.ogg
	cuetag.sh "$CUE" *split\).ogg
	exit
	;;

*.tta*)
	mkdir split
	shnsplit -d split -f "$CUE" -o "flac flac -V --best -o %f -" *.tta -t "%n %p - %t"
	rm -f split/00*pregap*
	cuetag.sh "$CUE" split/*.flac
	exit
	;;

*.wv*)
	mkdir split
	shnsplit -d split -f "$CUE" -o "flac flac -V --best -o %f -" *.wv -t "%n %p - %t"
	rm -f split/00*pregap*
	cuetag.sh "$CUE" split/*.flac
	exit
	;;

*.wav*)
	mkdir split
	shnsplit -d split -f "$CUE" -o "flac flac -V --best -o %f -" *.wav -t "%n %p - %t"
	rm -f split/00*pregap*
	cuetag.sh "$CUE" split/*.flac
	exit
	;;

* )
	echo "Error: Found no files to split!"
	echo "	   --> APE, FLAC, MP3, OGG, TTA, WV, WAV"
esac
exit
