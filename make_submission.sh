rm -rf 7/
mkdir -p 7
cp $@ ./7/
tar -zcvf 7.tar.gz 7/
rm -rf 7

