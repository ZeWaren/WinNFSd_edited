CC = g++
LINK = g++
CCFLAGS = -c -Wall
#CCFLAGS += -g -D_DEBUG
LIBS = -l ws2_32
OBJPATH = obj
OBJS = $(OBJPATH)\DatagramSocket.o $(OBJPATH)\FileTable.o $(OBJPATH)\MountProg.o $(OBJPATH)\NFS2Prog.o $(OBJPATH)\NFS3Prog.o $(OBJPATH)\NFSProg.o $(OBJPATH)\PortmapProg.o $(OBJPATH)\RPCProg.o $(OBJPATH)\RPCServer.o $(OBJPATH)\ServerSocket.o $(OBJPATH)\Socket.o $(OBJPATH)\SocketStream.o $(OBJPATH)\winnfsd.o
TARGET = winnfsd.exe

all: $(TARGET)
	@echo done

$(TARGET): $(OBJS)
	$(LINK) -o $(TARGET) $(OBJS) $(LIBS)

$(OBJPATH)\DatagramSocket.o: DatagramSocket.cpp DatagramSocket.h
	$(CC) $(CCFLAGS) -o $@ DatagramSocket.cpp

$(OBJPATH)\FileTable.o: FileTable.cpp FileTable.h
	$(CC) $(CCFLAGS) -o $@ FileTable.cpp

$(OBJPATH)\MountProg.o: MountProg.cpp MountProg.h
	$(CC) $(CCFLAGS) -o $@ MountProg.cpp

$(OBJPATH)\NFS2Prog.o: NFS2Prog.cpp NFS2Prog.h
	$(CC) $(CCFLAGS) -o $@ NFS2Prog.cpp

$(OBJPATH)\NFS3Prog.o: NFS3Prog.cpp NFS3Prog.h
	$(CC) $(CCFLAGS) -o $@ NFS3Prog.cpp

$(OBJPATH)\NFSProg.o: NFSProg.cpp NFSProg.h
	$(CC) $(CCFLAGS) -o $@ NFSProg.cpp

$(OBJPATH)\PortmapProg.o: PortmapProg.cpp PortmapProg.h
	$(CC) $(CCFLAGS) -o $@ PortmapProg.cpp

$(OBJPATH)\RPCProg.o: RPCProg.cpp RPCProg.h
	$(CC) $(CCFLAGS) -o $@ RPCProg.cpp

$(OBJPATH)\RPCServer.o: RPCServer.cpp RPCServer.h
	$(CC) $(CCFLAGS) -o $@ RPCServer.cpp

$(OBJPATH)\ServerSocket.o: ServerSocket.cpp ServerSocket.h
	$(CC) $(CCFLAGS) -o $@ ServerSocket.cpp

$(OBJPATH)\Socket.o: Socket.cpp Socket.h
	$(CC) $(CCFLAGS) -o $@ Socket.cpp

$(OBJPATH)\SocketStream.o: SocketStream.cpp SocketStream.h
	$(CC) $(CCFLAGS) -o $@ SocketStream.cpp

$(OBJPATH)\winnfsd.o: winnfsd.cpp
	$(CC) $(CCFLAGS) -o $@ winnfsd.cpp

.PHONY: all clean

clean:
	del $(OBJPATH)\*.o
	del $(TARGET)
