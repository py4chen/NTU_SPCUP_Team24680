all:
	raw2wav ./record_001 -s 44.1
	raw2wav ./record_002 -s 44.1
	raw2wav ./record_003 -s 44.1
	raw2wav ./record_004 -s 44.1
	raw2wav ./record_005 -s 44.1

clear-raw:
	rm -f ./record_001 ./record_002 ./record_003 ./record_004 ./record_005
clear-wav:
	rm -f ./record_001.wav ./record_002.wav ./record_003.wav ./record_004.wav ./record_005.wav
clear:	clear-raw clear-wav
