#include "../../Packet/packet.h"
#include <iostream>
template<typename PacketKind>
PacketKind ClientLogic::SendPacket(PacketIndex type, const char* buffer)
{
	switch (type)
	{
	case PacketIndex::Login:
	{
		PacketLogin packetLogin;
		memcpy(&packetLogin, buffer, sizeof(PacketLogin));
		send(_socket, (const char*)&packetLogin, packetLogin.header.headerSize, 0);
		break;
	}

	case PacketIndex::MAKE_ROOM:
	{
		PacketMakeRoom packetMakeRoom;
		memcpy(&packetMakeRoom, buffer, sizeof(PacketMakeRoom));
		send(_socket, (const char*)&packetMakeRoom, packetMakeRoom.header.headerSize, 0);
		RES_PacketMakeRoom resPacketMakeRoom;
		recv(_socket, (char*)&resPacketMakeRoom, sizeof(RES_PacketMakeRoom), 0);
		std::cout << resPacketMakeRoom.roomNum << " make " << std::endl;
		return (<PacketKind>)resPacketMakeRoom.roomNum;
	}

	case PacketIndex::ROOM_LIST:
	{
		PacketRoomList packetRoomList;
		send(_socket, (const char*)&packetRoomList, packetRoomList.header.headerSize, 0);
		RES_PacketRoomList resPacketRoomList;
		recv(_socket, (char*)&resPacketRoomList, sizeof(RES_PacketRoomList), 0);
		return resPacketRoomList;
	}

	default:
		break;
	}
}
