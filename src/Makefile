install: ../bin/preprocess ../bin/prepare ../bin/corr ../bin/hash ../bin/filter ../bin/sta ../bin/readins ../bin/errCor ../bin/postprocessor ../bin/asm ../bin/extend ../bin/combine ../bin/scaffold mt2
../bin/preprocess:preprocess.cpp
	g++ -o ../bin/preprocess preprocess.cpp
../bin/prepare:prepare.cpp
	g++ -o ../bin/prepare prepare.cpp -std=c++0x
../bin/corr:corr.cpp
	g++ -o ../bin/corr corr.cpp
../bin/hash:hash.cpp
	g++ -o ../bin/hash hash.cpp -mcmodel=medium -std=c++0x
../bin/filter:filter.cpp
	g++ -o ../bin/filter filter.cpp -mcmodel=medium
../bin/sta:sta.cpp
	g++ -o ../bin/sta sta.cpp -std=c++0x
../bin/readins:readins.cpp
	g++ -o ../bin/readins readins.cpp -mcmodel=medium -std=c++0x 
../bin/errCor:errCor.cpp
	g++ -o ../bin/errCor errCor.cpp -mcmodel=medium -std=c++0x -lpthread 
../bin/postprocessor:postprocessor.cpp
	g++ -o ../bin/postprocessor postprocessor.cpp -mcmodel=medium -std=c++0x 
../bin/asm:asm.cpp
	g++ -o ../bin/asm asm.cpp -std=c++0x -mcmodel=medium 
../bin/extend:extend.cpp
	g++ -o ../bin/extend extend.cpp -std=c++0x -mcmodel=medium 
../bin/combine:combine.cpp
	g++ -o ../bin/combine combine.cpp -std=c++0x -mcmodel=medium 
../bin/scaffold:scaffold.cpp
	g++ -o ../bin/scaffold scaffold.cpp -std=c++0x -mcmodel=medium 
mt2:
	sh compile.sh
