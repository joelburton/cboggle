create: Word-List.txt
	gcc -O3 -o dawg-maker Blitzkrieg_Trie_Attack_Dawg_Creator_Custom_Character_Set.c
	gcc -O3 -o crc-maker Compile-CRC-32-Lookup-Table.c
	./crc-maker
	./dawg-maker
	mv Traditional_Dawg_For_Word-List.dat ../words.dat

clean:
	rm -f crc-maker
	rm -f dawg-maker
	rm -f Traditional_Dawg_Explicit_Text_For_Word-List.txt
	rm -f CRC-32.dat
