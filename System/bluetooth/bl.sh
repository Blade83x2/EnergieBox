#!/bin/bash
shopt -os nounset pipefail errexit errtrace
clear
if [ "$(id -u)" != "0" ]; then
	echo -e "\033[1;31mAufruf nur mit sudo $0\033[0m \n"
	exit 0;
fi

#rfkill block bluetooth
sleep 1
#rfkill unblock bluetooth
adapter="hci0"
programm_version="1.0alpha"

if [ -d /usr/share/doc/bluez/ ] || [ -d /usr/share/doc/bluez-utils/ ]; then
	echo -e "\033[1mProgramm zum Einrichten von Bluetooth Geräten über die Konsole.\nUnterstützt Multi Bluetooth Adapter.\033[0m\n"
	adapter_mac=$(hciconfig -a $adapter | grep "BD Address: ")
	adapter_mac=$(echo "${adapter_mac:13:17}")
	echo -e "Benutzer: \033[1m\t$USER\033[0m \nHostname: \033[1m\t$HOSTNAME\033[0m"
	echo -e "Adapter: \033[1m\t$adapter ($adapter_mac)\033[0m"
	echo -e "Manufacturer: \033[1m\tJohannes Krämer <info@cplusplus-development.de>\033[0m"
	echo -e "Version: \033[1m\t$programm_version\033[0m"
	echo -e "Datum: \033[1m\t\t25.12.2023\033[0m"
	

	while [ true ]; do
		sleep 0.1
		echo -e "\n1)\t\033[37mService: Status\033[0m"
		echo -e "2)\t\033[32mService: Start\033[0m"
		echo -e "3)\t\033[31mService: Stop\033[0m"
		echo -e "4)\t\033[36mService: Restart\033[0m"
		echo -e "5)\t\033[32mAdapter sichtbar\033[0m"
		echo -e "6)\t\033[31mAdapter unsichtbar\033[0m"
		echo -e "7)\t\033[37mAdapter Informationen\033[0m"
		echo -e "8)\t\033[33mAdapter wählen\033[0m"
		echo -e "9)\t\033[35mVerfügbare Geräte anzeigen\033[0m"
		echo -e "10)\t\033[32mGerät koppeln\033[0m"
		echo -e "11)\t\033[31mGerät entkoppeln\033[0m"
		echo -e "12)\tBeenden\n"
		read -p "Auswahl: " -n 2 choice
		echo -e "\n"
		case $choice in
			'1')
				/etc/init.d/bluetooth status
				;;
			'2')
				sudo /etc/init.d/bluetooth start
				/etc/init.d/bluetooth status
				;;
			'3')	
				sudo /etc/init.d/bluetooth stop
				/etc/init.d/bluetooth status
				;;
			'4')
				sudo /etc/init.d/bluetooth restart
				/etc/init.d/bluetooth status
				;;
			'5')
				sudo hciconfig $adapter piscan
				sleep 1
				echo -e "\033[32mAdapter wird angezeigt auf anderen Geräten.\033[0m"
				;;
			'6')
				sudo hciconfig $adapter noscan
				sleep 1
				echo -e "\033[31mAdapter wird nicht angezeigt auf anderen Geräten.\033[0m"
				;;
			'7')
				sleep 1
				echo -e "\033[32mAlle verfügbaren Bluetooth Adapter:\n\033[0m"
				hcitool dev
				;;
			'8')
				echo -e "\033[32mUSB Adapter auswählen:\n\033[0m"
				hcitool dev
				echo -e "\n"
				read -p "Adapter wählen (z.B. hci0): " adapter_name
				adapter=$(echo $adapter_name)
				mac=$(hciconfig -a $adapter_name | grep "BD Address: ")
				mac=$(echo "${mac:13:17}")
				echo -e "Adapter $adapter_name, MAC: $mac"
				bluetoothctl <<< $'select '$mac
				sleep 2
				/etc/init.d/bluetooth restart
				sleep 1
				rfkill block bluetooth
				sleep 1
				rfkill unblock bluetooth
				;;
			'9')
				hcitool scan
				;;
			"10")
				hcitool scan
				echo -e "\n"
				read -p "Welches Gerät soll gekoppelt werden? (Zeilennummer erwartet z.B. 1) " -n 1 device
				echo -e "\n"
				ADDR=$(hcitool scan | sed -n '/'"$device"'/p' | cut -c 2-19);
				./btcon.sh "$ADDR"
				;;
			"11")
				curr_device_mac=($(hcitool dev | grep 'hci' | cut -c 7-));
				echo -e "Derzeit gekoppelte Geräte: $curr_device_mac-----\n"
				delcount=0;
				for adapter_addr in ${curr_device_mac[@]}; do
					if [ -d /var/lib/bluetooth/$adapter_addr ]; then				
						if [ ${#adapter_addr} == 17 ]; then						
							for absolute_device_path in /var/lib/bluetooth/$adapter_addr/*/info; do
								while IFS== read -r name value
								do
									if [[ $name == "Name" ]]; then
										((delcount=delcount+1))
										echo "$delcount)	$adapter_addr -> $value" 
									fi
								done < $absolute_device_path
							done
						fi
					fi
				done			
				echo -e "\n"
				read -p "Soll ein Gerät entkoppelt und gelöscht werden? (j/n) " -n 1 sure
				echo -e "\n"
				if [ $sure = 'j' ] || [ $sure = 'J' ] || [ $sure = 'y' ] || [ $sure = 'Y' ]; then
						read -p "Welche Nummer hat dieses Gerät? " -n 1 deletion
						echo -e "\n"
						delcount=0;
						for adapter_addr in ${curr_device_mac[@]}; do
							if [ -d /var/lib/bluetooth/$adapter_addr ]; then				
								if [ ${#adapter_addr} == 17 ]; then						
									for absolute_device_path in /var/lib/bluetooth/$adapter_addr/*/info; do
										while IFS== read -r name value
										do
											if [[ $name == "Name" ]]; then
												((delcount=delcount+1))
												if [[ $delcount == $deletion ]]; then
													device_mac_to_delete=$(echo "${absolute_device_path:37:17}")
													echo "Remove: $adapter_addr:$device_mac_to_delete -> $value" 
													bluetoothctl <<< $'remove '$device_mac_to_delete
													echo -e "\033[31m\n$device_mac_to_delete wurde gelöscht!\033[0m"
												fi
											fi
										done < $absolute_device_path
									done
								fi
							fi
						done	

					#	MAC_deleted=$(sed '/'"$deletion"'/p' /var/lib/bluetooth/$MAC/names | cut -c 19-)

					#	touch /var/lib/bluetooth/$MAC/names_temp

					#	chmod 777 /var/lib/bluetooth/$MAC/names
					#	chmod 777 /var/lib/bluetooth/$MAC/names_temp

					#	sed ''"$deletion"'d' /var/lib/bluetooth/$MAC/names > /var/lib/bluetooth/$MAC/names_temp
					#	cat /var/lib/bluetooth/$MAC/names_temp > /var/lib/bluetooth/$MAC/names

					#	rm /var/lib/bluetooth/$MAC/names_temp

					#	echo -e "\033[32m$MAC_deleted might be Unpaired.\033[0m"

				fi
				;;

			"12")
				echo -e "\nByebye!"
			
				clear
				exit 1
				;;

			*)
				echo -e "Falsche Auswahl!\n"
				;;
		esac

	done

else

	echo "\033[31mThere is no any Bluetooth Support installed, or it is broken."
	read -p "Do you want to install one ? (y/n) " -n 1 check
	echo -e "\n"

	if [ $check = 'y' ] || [ $check = 'Y' ]; then

		apt-get update && apt-get install bluetooth > /dev/null
		wait
		echo -e "\nPlease, consider re-executing this script.\n\n"

	else

		clear

	fi

fi

echo 0
