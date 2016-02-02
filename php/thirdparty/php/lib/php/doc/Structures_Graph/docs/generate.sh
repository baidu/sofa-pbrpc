#!/bin/sh
(cd ..; tar czf docs/arch.tgz "{arch}")
rm -Rf "../{arch}"
rm -Rf ./html
mkdir -p ./html
phpdoc --directory ../Structures,./tutorials --target ./html --title "Structures_Graph Documentation" --output "HTML:frames" --defaultpackagename structures_graph --defaultcategoryname structures --pear 
(cd ..; tar --absolute-names -xzf docs/arch.tgz)
#rm arch.tgz
