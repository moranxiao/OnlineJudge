.PHONY:all
all:
	@cd ./compile_server;\
	make;\
	cd -;\
	cd ./oj_server;\
	make;\
	cd -

.PHONY:output
output:
	@mkdir output;\
	cd ./compile_server;\
	cp compile_server ../output/;\
	cp -r temp ../output/;\
	cd ..;\
	cd ./oj_server;\
	cp oj_server ../output/;\
	cp -r cnf ../output/;\
	cp -r questions ../output/;\
	cp -r template_html ../output/;\
	cp -r wwwroot ../output/

.PHONY:clean
clean:
	@cd ./compile_server;\
	make clean;\
	cd ..;\
	cd ./oj_server;\
	make clean;\
	cd ..;\
	rm -rf output
	