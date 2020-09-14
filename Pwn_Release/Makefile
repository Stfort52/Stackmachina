pwn: source.c
	gcc source.c -s -o stackmachina_release -z relro -z now -fstack-protector-all
rev: source.c binary.sm
	gcc source.c -s -o stackmachina_rev_release -z relro -z now -fstack-protector-all -pie -fPIC -D_fortify_source=2 -O2