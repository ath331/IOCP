/*
Dummy Client를 만들어서 서버성능을 테스트해보기위한 프로젝트.
실제 사용자들의 활동을 따라 할 수있다. (로그인, 방만들기, 방접속, 채팅등등)
클라이언트가 접속을 종료한 경우는 구현하지 않았다.
*/

#include <iostream>
#include "Client/TestClient.h"

using namespace std;

int main()
{
	cout << "Dummy Client Project" << endl;
	TestClient testClient;
	testClient.RunTestClient();
}