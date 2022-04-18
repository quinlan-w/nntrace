all: txt2bin bin2json

txt2bin:
	g++ txt2bin.cpp -Iinclude -o txt2bin
bin2json:
	g++ bin2json.cpp -Iinclude -o bin2json
clean:
	rm bin2json txt2bin *.json *.bin
