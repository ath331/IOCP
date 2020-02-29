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
	if (buffer == NULL)
		return 0;

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
		int recvLen = recv(_socket, (char*)&resPacketMakeRoom, sizeof(RES_PacketMakeRoom), 0);
		while (recvLen < sizeof(RES_PacketMakeRoom))
		{
			recvLen += recv(_socket, (char*)&resPacketMakeRoom, 1, 0);
		}
		std::cout << resPacketMakeRoom.roomNum << " make " << std::endl;
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
		memcpy(&packetCloseRoom, buffer, sizeof(PacketCloseRoom));
		send(_socket, (const char*)&packetCloseRoom, packetCloseRoom.header.headerSize, 0);
		return 0;
	}
	case PacketIndex::SEND_MESSAGE:
	{
		PacketSendMessage packetSendMessage;
		memcpy(&packetSendMessage, buffer, sizeof(PacketSendMessage));
		send(_socket, (const char*)&packetSendMessage, packetSendMessage.header.headerSize, 0);
		return 0;
	}
	default:
		break;
	}
}
