CC = mipsel-linux-uclibc-gcc
#CC = gcc

CFLAGS = -g -lm -lpthread -g -O2 -Wall -std=gnu99 \
-D_GNU_SOURCE -Wpointer-arith -Wcast-align -Wstrict-prototypes \
-Wshadow -Waggregate-return -Wmissing-prototypes -Wnested-externs -Wsign-compare -Wcast-qual -DDVBPSI_DIST
SRC = ./src
SRC_TB = $(SRC)/tables
SRC_DESCRIPTORS = $(SRC)/descriptors
EXEC = ./examples
EXEC_DVB = $(EXEC)/dvbinfo


SRC_SOURCES = $(wildcard  $(SRC)/*.c $(SRC_TB)/*.c $(SRC_DESCRIPTORS)/*.c )#$(EXEC_DVB)/*.c)

OBJ = $(patsubst %.c, %.o, $(SRC_SOURCES))

ALL:decode_pmt decode_pat decode_sdt dvbinfo

SRC_PMT = $(EXEC)/decode_pmt.c
OBJ_PMT = $(patsubst %.c,%.o, $(SRC_PMT))
decode_pmt: $(OBJ_PMT)
	$(CC) $(OBJ_PMT) $(SRC_SOURCES) $(CFLAGS) -o $@


SRC_SDT = $(EXEC)/decode_sdt.c
OBJ_SDT = $(patsubst %.c,%.o, $(SRC_SDT))
decode_sdt: $(OBJ_SDT)
	$(CC) $(OBJ_SDT) $(SRC_SOURCES) $(CFLAGS) -o $@


SRC_PAT = $(EXEC)/decode_pat.c 
OBJ_PAT = $(patsubst %.c,%.o, $(SRC_PAT))
decode_pat: $(OBJ_PAT)
	$(CC) -o $@ $(OBJ_PAT) $(SRC_SOURCES) $(CFLAGS)


SRC_DVBINFO = $(EXEC_DVB)/*.c
OBJ_DVBINFO = $(patsubst %.c,%.o, $(SRC_DVBINFO))
dvbinfo: $(OBJ_DVBINFO)
	$(CC) $(wildcard $(SRC_DVBINFO)) $(SRC_SOURCES) $(CFLAGS) -o $@



	
clean:
	-rm -rf decode-mpeg $(OBJ) $(OBJ_DVBINFO) $(OBJ_SDT) $(OBJ_PAT) $(OBJ_PMT)
	
