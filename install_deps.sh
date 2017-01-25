distro=$(lsb_release -si)
distro=`echo "$distro" | tr '[:upper:]' '[:lower:]'`

if [[ $distro == *"arch"* || $distro == *"manjaro"* ]]
then
	sudo pacman -S base-devel sdl2 sdl2_net sdl2_mixer openssl zlib libcurl-gnutls
elif [[ $distro == *"ubuntu"* || $distro == *"debian"* ]]
then
	apt-get install build-essential libsdl2-dev libsdl2-net-dev libsdl2-mixer-dev libssl-dev zlib1g-dev libcurl4-gnutls-dev
fi
