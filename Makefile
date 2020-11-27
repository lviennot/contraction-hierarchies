
build: _build
	cd $<; make -j

_example: test_data/road_corsica.txt test_data/road_corsica_nodes.txt
	_build/main $^ 8.0 > $@

_build:
	mkdir -p $@
	cd $@; ln -sf ../test_data; cmake ..

unit: build
	_build/unit | tee > /tmp/_unit_out
	if [ -f _unit_out ]; then diff /tmp/_unit_out _unit_out; fi

_unit_out:
	_build/unit > $@

clean:
	rm -f *.o src/*~ *~
	rm -fr *.o.dSYM _*

%.force:
	rm -f $*; make $*
