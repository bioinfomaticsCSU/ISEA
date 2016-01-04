which perl || { echo "perl is not found"; exit 1; }
echo "perl is ok"
which python || { echo "python is not found"; exit 1; }
echo "python is ok"
#echo "installing ISEA ..."
#g++ -o ../bin/preprocess preprocess.cpp || { exit 1; }
#g++ -o ../bin/prepare prepare.cpp -std=c++0x || { exit 1; }
#g++ -o ../bin/corr corr.cpp || { exit 1; }
#g++ -o ../bin/hash hash.cpp -mcmodel=medium -std=c++0x || { exit 1; }
#g++ -o ../bin/filter filter.cpp -mcmodel=medium || { exit 1; }
#g++ -o ../bin/sta sta.cpp -mcmodel=medium -std=c++0x || { exit 1; }
#g++ -o ../bin/readins readins.cpp -mcmodel=medium -std=c++0x || { exit 1; }
#g++ -o ../bin/errCor errCor.cpp -mcmodel=medium -std=c++0x -lpthread || { exit 1; }
#g++ -o ../bin/postprocessor postprocessor.cpp -mcmodel=medium -std=c++0x || { exit 1; }
#g++ -o ../bin/asm asm.cpp -std=c++0x -mcmodel=medium || { exit 1; }
#g++ -o ../bin/extend extend.cpp -std=c++0x -mcmodel=medium || { exit 1; }
#g++ -o ../bin/combine combine.cpp -std=c++0x -mcmodel=medium || { exit 1; }
#g++ -o ../bin/scaffold scaffold.cpp -std=c++0x -mcmodel=medium || { exit 1; }
#echo "Done"
echo "installing bowtie2"
if [ ! -d "../bin/bowtie2-2.2.1/" ]; then
  echo "Fold ../bin/bowtie2-2.2.1/ does not exist."
  exit 1
fi
cd ../bin/bowtie2-2.2.1/ || { exit 1; }
make || { echo "make bowtie2 failed"; exit 1; }
echo "Done"
