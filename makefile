render: main.cpp
	g++ -o render main.cpp
image.ppm: render
	./render
run: render
	./render
view: image.ppm
	feh -. image.ppm
clean:
	rm image.ppm
	rm render
