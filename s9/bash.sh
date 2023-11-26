if [ "$#" -ne 1 ]
then echo "Numarul de argumente nu este cel potrivit"
     exit 1
fi

contor=0

#Pentru a căuta un anumit tipar la începutul liniei, se poate folosi caracterul '^'. Pentru a căuta anumite informații la finalul liniei, ne putem folosi de simbolul '$'.

while read  line
do
  if echo "$line" | grep -E '^[A-Z][A-Za-z,?!\. ]*' | grep -v ', si' | grep -E '(\?|\!|\.)$'; then
    contor=$((contor+1))
  fi

done

echo $contor