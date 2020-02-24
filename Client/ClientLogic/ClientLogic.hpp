#include "../../Packet/packet.h"
#include <iostream>

template<typename PacketKind>
PacketKind ClientLogic::SendPacket(PacketIndex type, const char* buffer)
{
	switch (type)
	{
	case PacketIndex::ROOM_LIST:
	{
		PacketRoomList packetRoomList;
		send(_socket, (const char*)&packetRoomList, packetRoomList.header.headerSize, 0);
		RES_PacketRoomList resPacketRoomList;
		recv(_socket, (char*)&resPacketRoomList, resPacketRoomList.header.headerSize, 0);
		return resPacketRoomList;
	}

	default:
		break;
	}
}

template<>
int ClientLogic::SendPacket(PacketIndex type, const char* buffer)
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
		Sleep(10); 
		//★TODO : recv가 제대로 안되서 패킷을 다받지 못하므로 size까지 다읽도록 변경
		//Sleep은 패킷이 다 도착하면 읽도록 임시방편
		recv(_socket, (char*)&resPacketMakeRoom, sizeof(RES_PacketMakeRoom), 0);
		std::cout << resPacketMakeRoom.roomNum - 1 << " make " << std::endl;
		return resPacketMakeRoom.roomNum;
	}

	case PacketIndex::ENTER_ROOM:
	{
		PacketEnterRoom packetEnterRoom;
		memcpy(&packetEnterRoom, buffer, sizeof(PacketEnterRoom));
		send(_socket, (const char*)&packetEnterRoom, packetEnterRoom.header.headerSize, 0);
		return 0;
	}

	case PacketIndex::CLOSE_ROOM:
	{
		PacketCloseRoom packetCloseRoom;
		send(_socket, (const char*)&packetCloseRoom, packetCloseRoom.header.headerSize, 0);
		return 0;
	}
	case PacketIndex::SEND_MESSAGE:
	{
		PacketSendMessage packetSendMessage;
		memcpy(&packetSendMessage, buffer, sizeof(PacketSendMessage));
		send(_socket, (const char*)&packetSendMessage, packetSendMessage.header.headerSize, 0);
	}
	default:
		break;
	}
}
