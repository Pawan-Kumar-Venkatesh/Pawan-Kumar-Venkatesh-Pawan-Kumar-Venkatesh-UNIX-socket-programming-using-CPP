.PHONY:all
all:
	g++ -std=c++11 central.cpp -o central
	g++ -std=c++11 serverT.cpp -o serverT
	g++ -std=c++11 serverS.cpp -o serverS
	g++ -std=c++11 serverP.cpp -o serverP
	g++ -std=c++11 clientA.cpp -o clientA
	g++ -std=c++11 clientB.cpp -o clientB

.PHONY:central
serverC:
	./central

.PHONY:serverT
serverT:
	./serverT

.PHONY:serverS
serverS:
	./serverS

.PHONY:serverP
serverP:
	./serverP

.PHONY:clean
clean:
	rm -rf central
	rm -rf serverT
	rm -rf serverS
	rm -rf serverP
	rm -rf clientA
	rm -rf clientB
