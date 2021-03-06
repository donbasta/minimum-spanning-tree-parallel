if ["$#" -ne 1]; then
    echo 'Usage: ./test.sh <input_file>'
    exit
fi

input=$1
filename="basename $1"

cat $1 | ./MST_OpenMP > output_mp/tmp
sed '$d' output_mp/tmp > output_mp/$filename
rm -f output_mp/tmp