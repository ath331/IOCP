//#include "../../Packet/packet.h"
//#include <iostream>
//
//template <typename PacketKind>
//PacketKind ClientLogic::RecvPacket(PacketIndex type)
//{
//	switch (type)
//	{
//    case PacketIndex::DB_INSERT_DATA:
//    {
//        std::fill_n(buf, 1024, 0);
//        int recvLen = recv(_socket,(char*)buf,sizeof(PacketDBInsertData),0);
//        while (recvLen < sizeof(PacketDBInsertData))
//        {
//            recvLen += recv(_socket, (char*)buf[recvLen], 1, 0);
//        }
//        return (PacketKind*)&buf;
//    }
//    default:
//        break;
//	}
//}
