if [ "$#" -ne 1 ]
then echo "Numarul de argumente nu este cel potrivit"
     exit 1
fi

contor=0

while read line
  do
  if echo "$line" | grep '^[A-Z][A-Za-z,?!\. ]*' | grep -v ', si' | grep -E '(\?|\!|\.)$'
  then
    ((contor++))
  fi


done
echo $contor




