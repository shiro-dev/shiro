rm -rf /docker-install/i686-elf-tools-linux/
unzip /docker-install/i686-elf-tools-linux.zip -d /docker-install/i686-elf-tools-linux

if cat ~/.bashrc | grep "i686-elf"; then
  echo "OK";
else
  echo "export PATH=/docker-install/i686-elf-tools-linux/bin:$PATH" >> ~/.bashrc
fi