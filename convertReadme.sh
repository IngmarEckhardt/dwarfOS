#!/bin/sh

echo 'const __attribute__((__progmem__)) char readme_data[] =' > dwarf-os/readme_data.h
while IFS= read -r line
do
  # Escape double quotes and backslashes
  line=$(echo "$line" | sed 's/\\/\\\\/g' | sed 's/"/\\"/g')
  printf '"%s\\n"\n' "$line" >> dwarf-os/readme_data.h
done < README.md
echo ';' >> dwarf-os/readme_data.h