clear

#test file
testFile=mp3_test.c

#directory to store build/compiled files
buildDir=../untracked/build

#directory for avr-lcd source files
lcdDir=source/lcd

#directory for avr-fat source files
fatDir=source/fat

#directory for avr-sdcard source files
sdDir=source/sd

#directory for avr-general source files
genDir=source/gen

#directory for source files
sourceDir=source

#directory for source files
mp3Dir=source/mp3

#directory for test files
testDir=test

#make build directory if it doesn't exist
mkdir -p -v $buildDir


t=0.25
# -g = debug, -Os = Optimize Size
Compile=(avr-gcc -Wall -g -Os -I "includes/fat" -I "includes/sd" -I "includes/gen" -I "includes/lcd" -I "includes/mp3" -DF_CPU=16000000 -mmcu=atmega1280 -c -o)
Link=(avr-gcc -Wall -g -mmcu=atmega1280 -o)
IHex=(avr-objcopy -j .text -j .data -O ihex)


echo -e "\n>> COMPILE: "${Compile[@]}" "$buildDir"/test.o " $testDir"/"$testFile
"${Compile[@]}" $buildDir/test.o $testDir/$testFile
status=$?
sleep $t
if [ $status -gt 0 ]
then
    echo -e "error compiling TEST"
    echo -e "program exiting with code $status"
    exit $status
else
    echo -e "Compiling TEST successful"
fi

echo -e "\n>> COMPILE: "${Compile[@]}" "$buildDir"/mp3.o " $mp3Dir"/mp3.c"
"${Compile[@]}" $buildDir/mp3.o $mp3Dir/mp3.c
status=$?
sleep $t
if [ $status -gt 0 ]
then
    echo -e "error compiling MP3.C"
    echo -e "program exiting with code $status"
    exit $status
else
    echo -e "Compiling MP3.C successful"
fi

echo -e "\n>> COMPILE: "${Compile[@]}" "$buildDir"/lcd_base.o " $lcdDir"/lcd_base.c"
"${Compile[@]}" $buildDir/lcd_base.o $lcdDir/lcd_base.c
status=$?
sleep $t
if [ $status -gt 0 ]
then
    echo -e "error compiling LCD_BASE.C"
    echo -e "program exiting with code $status"
    exit $status
else
    echo -e "Compiling LCD_BASE.C successful"
fi

echo -e "\n>> COMPILE: "${Compile[@]}" "$buildDir"/lcd_sf.o " $lcdDir"/lcd_sf.c"
"${Compile[@]}" $buildDir/lcd_sf.o $lcdDir/lcd_sf.c
status=$?
sleep $t
if [ $status -gt 0 ]
then
    echo -e "error compiling LCD_SF.C"
    echo -e "program exiting with code $status"
    exit $status
else
    echo -e "Compiling LCD_SF.C successful"
fi

echo -e "\n>> COMPILE: "${Compile[@]}" "$buildDir"/fat.o "$fatDir"/fat.c"
"${Compile[@]}" $buildDir/fat.o $fatDir/fat.c
status=$?
sleep $t
if [ $status -gt 0 ]
then
    echo -e "error compiling FAT.C"
    echo -e "program exiting with code $status"
    exit $status
else
    echo -e "Compiling FAT.C successful"
fi


echo -e "\n>> COMPILE: "${Compile[@]}" "$buildDir"/fat_bpb.o "$fatDir"/fat_bpb.c"
"${Compile[@]}" $buildDir/fat_bpb.o $fatDir/fat_bpb.c
status=$?
sleep $t
if [ $status -gt 0 ]
then
    echo -e "error compiling FAT_BPB.C"
    echo -e "program exiting with code $status"
    exit $status
else
    echo -e "Compiling FAT_BPB.C successful"
fi


echo -e "\n>> COMPILE: "${Compile[@]}" "$buildDir"/fat_to_sd.o "$fatDir"/fat_to_sd.c"
"${Compile[@]}" $buildDir/fat_to_sd.o $fatDir/fat_to_sd.c
status=$?
sleep $t
if [ $status -gt 0 ]
then
    echo -e "error compiling FAT_TO_SD.C"
    echo -e "program exiting with code $status"
    exit $status
else
    echo -e "Compiling FAT_TO_SD.C successful"
fi


echo -e "\n>> COMPILE: "${Compile[@]}" "$buildDir"/spi.o "$genDir"/spi.c"
"${Compile[@]}" $buildDir/spi.o $genDir/spi.c
status=$?
sleep $t
if [ $status -gt 0 ]
then
    echo -e "error compiling SPI.C"
    echo -e "program exiting with code $status"
    exit $status
else
    echo -e "Compiling SPI.C successful"
fi


echo -e "\n>> COMPILE: "${Compile[@]}" "$buildDir"/prints.o "$genDir"/prints.c"
"${Compile[@]}" $buildDir/prints.o $genDir/prints.c
status=$?
sleep $t
if [ $status -gt 0 ]
then
    echo -e "error compiling PRINTS.C"
    echo -e "program exiting with code $status"
    exit $status
else
    echo -e "Compiling PRINTS.C successful"
fi


echo -e "\n>> COMPILE: "${Compile[@]}" "$buildDir"/usart0.o "$genDir"/usart0.c"
"${Compile[@]}" $buildDir/usart0.o $genDir/usart0.c
status=$?
sleep $t
if [ $status -gt 0 ]
then
    echo -e "error compiling USART0.C"
    echo -e "program exiting with code $status"
    exit $status
else
    echo -e "Compiling USART0.C successful"
fi


echo -e "\n>> COMPILE: "${Compile[@]}" "$buildDir"/sd_spi_base.o "$sdDir"/sd_spi_base.c"
"${Compile[@]}" $buildDir/sd_spi_base.o $sdDir/sd_spi_base.c
status=$?
sleep $t
if [ $status -gt 0 ]
then
    echo -e "error compiling SD_SPI_BASE.C"
    echo -e "program exiting with code $status"
    exit $status
else
    echo -e "Compiling SD_SPI_BASE.C successful"
fi


echo -e "\n>> COMPILE: "${Compile[@]}" "$buildDir"/sd_spi_rwe.o "$sdDir"/sd_spi_rwe.c"
"${Compile[@]}" $buildDir/sd_spi_rwe.o $sdDir/sd_spi_rwe.c
status=$?
sleep $t
if [ $status -gt 0 ]
then
    echo -e "error compiling SD_SPI_RWE.C"
    echo -e "program exiting with code $status"
    exit $status
else
    echo -e "Compiling SD_SPI_RWE.C successful"
fi


echo -e "\n>> LINK: "${Link[@]}" "$buildDir"/test.elf "$buildDir"/_test.o  "$buildDir"/spi.o "$buildDir"/sd_spi_base.o "$buildDir"/sd_spi_rwe.o "$buildDir"/usart0.o "$buildDir"/prints.o "$buildDir"/fat_bpb.o "$buildDir"/fat.o "$buildDir"/fat_to_sd.o" "$buildDir"/lcd_base.o" "$buildDir"/lcd_sf.o" "$buildDir"/mp3.o
"${Link[@]}" $buildDir/test.elf $buildDir/test.o $buildDir/spi.o $buildDir/sd_spi_base.o $buildDir/sd_spi_rwe.o $buildDir/usart0.o $buildDir/prints.o $buildDir/fat.o $buildDir/fat_bpb.o $buildDir/fat_to_sd.o $buildDir/lcd_base.o $buildDir/lcd_sf.o $buildDir/mp3.o
status=$?
sleep $t
if [ $status -gt 0 ]
then
    echo -e "error during linking"
    echo -e "program exiting with code $status"
    exit $status
else
    echo -e "Linking successful. Output in TEST.ELF"
fi



echo -e "\n>> GENERATE INTEL HEX File: "${IHex[@]}" "$buildDir"/test.elf "$buildDir"/test.hex"
"${IHex[@]}" $buildDir/test.elf $buildDir/test.hex
status=$?
sleep $t
if [ $status -gt 0 ]
then
    echo -e "error generating HEX file"
    echo -e "program exiting with code $status"
    exit $status
else
    echo -e "HEX file successfully generated. Output in TEST.HEX"
fi



echo -e "\n>> DOWNLOAD HEX FILE TO AVR"
echo "avrdude -p atmega1280 -c dragon_jtag -U flash:w:test.hex:i -P usb"
avrdude -p atmega1280 -c dragon_jtag -U flash:w:$buildDir/test.hex:i -P usb
status=$?
sleep $t
if [ $status -gt 0 ]
then
    echo -e "error during download of HEX file to AVR"
    echo -e "program exiting with code $status"
    exit $status
else
    echo -e "Program successfully downloaded to AVR"
fi

