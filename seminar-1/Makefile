CC=/usr/bin/x86_64-w64-mingw32-gcc-11.2.0
BUILDDIR=build

all: test

build:
	mkdir ${BUILDDIR}
	touch ${BUILDDIR}/code
	touch ${BUILDDIR}/info
	
test: build
	${CC} -o app test_app.c -mwindows

parser: build
#	${CC} -o parser parse_pe.c -lgdi32
	${CC} -o ${BUILDDIR}/parser parser.c -mwindows

clean:
	rm -rf ${BUILDDIR}