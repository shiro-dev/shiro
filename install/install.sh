if [ls /install | grep "i686-elf-tools-linux.zip"]; then
  echo "OK";
else
  wget -P /docker-install http://nimbleking.com/shiro/downloads/i686-elf-tools-linux.zip
fi

rm -rf /docker-install/i686-elf-tools-linux/

unzip /docker-install/i686-elf-tools-linux.zip -d /docker-install/i686-elf-tools-linux

echo "export PATH=/docker-install/i686-elf-tools-linux/bin:$PATH" >> ~/.bashrc