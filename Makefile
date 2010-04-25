.PHONY: clean main

main:
	cd src/libsemblis; $(MAKE)
	cp src/libsemblis/libsemblis.so .
	cp src/libsemblis/libsemblis.so src/tests
	cd src/tests; $(MAKE)
	cd plugins; $(MAKE)

clean:
	cd src/libsemblis; $(MAKE) clean
	cd src/tests; $(MAKE) clean
	cd plugins; $(MAKE) clean
	rm -f libsemblis.so