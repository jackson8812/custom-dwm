#!/bin/bash

getdate(){
 date="$(date '+%b %d %a')"
 echo -ne "\uE265 ${date} "
}

gettime(){
 time="$(date '+%H %M')"
 echo -ne "\uE018${time}"
}

getvolume(){
 volume="$(ponymix get-volume)%"
 mute="$(ponymix is-muted)"
 if [[ "$mute" != 0 ]];  then
	echo -ne "\uE050${volume}"
 else
	echo -ne "\uE052 N/A"
 fi
 }

getbattery(){
 status="$(acpi -b | awk '{print $3}' | tr -d ',')"
 battery="$(acpi -b | awk '{print $4}' | tr -d '%,')"
 time="$(acpi -b | awk '{print $5}')"
 if [[ $status == Dis* ]]; then
	 if [[ "$battery" -le 10 ]]; then
		 echo -ne "\x04 \uE038${battery}% ${time} \x01" # close to dead (white on red)
 	 elif [[ "$battery" -le 50 ]]; then
		 echo -ne "\x05 \uE038${battery}% ${time} \x01" # warning (black on yellow)
	 else
		 echo -ne "\x03 \uE038${battery}% ${time} \x01" # normal (black on lime green)
	 fi
 else
	 if [[ "$battery" -le 70 ]]; then
		 echo -ne "\x05 \uE038${battery}% CHR ${time} \x01" # Not charged yet
	 else
		 echo -ne "\x03 \uE038${battery}% CHR ${time} \x01" # Close to being done
	 fi
 fi
}

getmusic(){
 pidof spotify >/dev/null
 if [[ $? -eq 0 ]] ; then
	echo -ne "\x03 \uE05c$(echo `dbus-send --print-reply --dest=org.mpris.MediaPlayer2.spotify /org/mpris/MediaPlayer2 org.freedesktop.DBus.Properties.Get string:'org.mpris.MediaPlayer2.Player' string:'Metadata'|egrep -A 2 "artist"|egrep -v "artist"|egrep -v "array"|cut -b 27-|cut -d '"' -f 1|egrep -v ^$` "-" `dbus-send --print-reply --dest=org.mpris.MediaPlayer2.spotify /org/mpris/MediaPlayer2 org.freedesktop.DBus.Properties.Get string:'org.mpris.MediaPlayer2.Player' string:'Metadata'|egrep -A 1 "title"|egrep -v "title"|cut -b 44-|cut -d '"' -f 1|egrep -v ^$`) \x01"
 fi


 pidof mpd >/dev/null
 if [[ $? -eq 0 ]] ; then
	if mpc status | grep playing >/dev/null ; then  # If mpd is playiny
		echo -ne "\x04 \uE05c$(echo `mpc current`) \x01"
	elif mpc status | grep paused >/dev/null ; then # if mpd is paused
		echo -ne "\x04 \uE05c$(echo 'MPD Paused') \x01"
	else
		echo -ne "\uE05c MPD Running but with No Music"
	fi
 fi
}

while true; do
	xsetroot -name "$(getvolume) $(getbattery) $(getmusic) $(getdate) $(gettime)"
	sleep 60
done &
