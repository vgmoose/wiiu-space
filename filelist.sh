#! /bin/bash
#
# Automatic resource file list generation
# Created by Dimok

outFile="./src/resources/filelist.h"
count_old=$(cat $outFile 2>/dev/null | tr -d '\n\n' | sed 's/[^0-9]*\([0-9]*\).*/\1/')

count=0

for i in $(find ./data/images/ ./data/sounds/ ./data/fonts/ -maxdepth 1 -type f)  #\( ! -printf "%f\n" \) | sort -f)
do
	files[count]=$i
	count=$((count+1))
done

#fi

if [ "$count_old" != "$count" ] || [ ! -f $outFile ]
then

echo "Generating filelist.h for $count files." >&2
cat <<EOF > $outFile
/****************************************************************************
 * Loadiine resource files.
 * This file is generated automatically.
 * Includes $count files.
 *
 * NOTE:
 * Any manual modification of this file will be overwriten by the generation.
 ****************************************************************************/
#ifndef _FILELIST_H_
#define _FILELIST_H_

typedef struct _RecourceFile
{
	const char *filename;
	const unsigned char   *DefaultFile;
	const unsigned int  &DefaultFileSize;
	unsigned char		   *CustomFile;
	unsigned int		    CustomFileSize;
} RecourceFile;

EOF

for i in ${files[@]}
do
	filename=${i%.*}
	extension=${i##*.}

  # For some reason, macOS and other Darwin-based operating systems contain a
  # find untility that outputs full file paths instead of singular file names.
  # To prevent this from screwing our program, we will remove the paths from the
  # entries returned by the above loop if we are running on a Darwin platform.
  if [[ $OSTYPE == darwin* ]]; then
    if [[ ${filename:0:15} == ./data/sounds// ]]; then
      filename=${filename:15}
    elif [[ ${filename:0:15} == ./data/images// ]]; then
      filename=${filename:15}
    elif [[ ${filename:0:14} == ./data/fonts// ]]; then
      filename=${filename:14}
    fi
  fi

	echo 'extern const unsigned char '$filename'_'$extension'[];' >> $outFile
	echo 'extern const unsigned int '$filename'_'$extension'_size;' >> $outFile
	echo '' >> $outFile
done

echo 'static RecourceFile RecourceList[] =' >> $outFile
echo '{' >> $outFile

for i in ${files[@]}
do
	filename=${i%.*}
	extension=${i##*.}

  # See above for documentation on what this does.
  if [[ $OSTYPE == darwin* ]]; then
    if [[ ${filename:0:15} == ./data/sounds// ]]; then
      filename=${filename:15}
    elif [[ ${filename:0:15} == ./data/images// ]]; then
      filename=${filename:15}
    elif [[ ${filename:0:14} == ./data/fonts// ]]; then
      filename=${filename:14}
    fi
  fi

	echo -e '\t{"'$filename'.'$extension'", '$filename'_'$extension', '$filename'_'$extension'_size, NULL, 0},' >> $outFile
done

echo -e '\t{NULL, NULL, 0, NULL, 0}' >> $outFile
echo '};' >> $outFile

echo '' >> $outFile
echo '#endif' >> $outFile

fi
