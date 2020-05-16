CC=g++
FLAGS=--std=c++17
GP=gprof
AS=nasm

dists:
	$(CC) $(FLAGS) -O3 dist.cpp -o dist
	./dist 100000
	find -name "*_dist.csv" -exec ./graph.py {} \;
	mogrify -quality 100 -density 300 -format jpg *.pdf
	mkdir -p results/dists
	mv *.jpg results/dists

asm:
	$(AS) -f elf64 xor_hash.s -o xor_hash.o
	$(AS) -f elf64 strcmp.s -o strcmp.o

meas:
	$(CC) $(FLAGS) $(OLVL) meas.cpp -o meas
	time -p ./meas

asmmeas: asm
	$(CC) $(FLAGS) $(OLVL) -DASMOPTIMIZATION meas.cpp xor_hash.o strcmp.o -o meas
	time -p ./meas

prof:
	$(CC) $(FLAGS) -pg meas.cpp -o meas
	./meas
	$(GP) meas gmon.out | python3 -m gprof2dot -s | dot -Tjpg -Gdpi=300 -o prof.jpg
	mkdir -p results/profs
	mv prof.jpg results/profs

asmprof: asm
	$(CC) $(FLAGS) -DASMOPTIMIZATION -pg meas.cpp xor_hash.o strcmp.o -o meas
	./meas
	$(GP) meas gmon.out | python3 -m gprof2dot -s | dot -Tjpg -Gdpi=300 -o asmprof.jpg
	mkdir -p results/profs
	mv asmprof.jpg results/profs

clean:
	rm -f dist meas gmon.out xor_hash.o strcmp.o *.csv *.pdf *.jpg
