// Client.cpp : Copyright (C) 2024 https://www.youtube.com/@ProgrammerCpp
#include <Library/TCP.h>
#include <iostream>

Library::TCP connect_to_server
( _In_ const std::string     & host_or_addr
, _In_ u_short           const port_number 
)
{
	std::cout << "サーバ[" << host_or_addr << "]へ接続中...";
	sockaddr_in name;
	auto s = Library::TCP::connect(host_or_addr, port_number, name);
	std::cout << '\n' << "サーバ=" << inet_ntoa(name.sin_addr) << ", ポート番号=" << ntohs(name.sin_port) << "に接続\n\n";
	return s;
}
void interact_with_server(const Library::TCP& s)
{
	for (std::stringstream stm;;) {
		std::cout << "文字列の入力: ";
		std::string command;
		if (std::getline(std::cin, command)) {
			s.send_message(command);
			std::cout << "受信した文字列: " << s.recv_message(stm) << '\n';
			if (command == ".")
				break;
		}
		else
			break;
	}
}
#undef	max
int main()
{
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 0), &d) == 0) {
		try {
			std::cout << "サーバのホスト名（またはIPアドレス）: ）";
			std::string host_or_addr;
			if (std::cin >> host_or_addr) {
				std::cout << "サーバのポート番号: ";
				u_short port_number;
				if ((std::cin >> port_number).ignore(std::numeric_limits<std::streamsize>::max(), '\n')) {
					interact_with_server(connect_to_server(host_or_addr, port_number));
					std::cout << "Enter キーを押してプログラム終了\n";
					std::cin.get();
				}
			}
		}
		catch (const std::exception& e) {
			std::cerr << e.what() << '\n';
		}
		WSACleanup();
	}
}
/*
https://detail.chiebukuro.yahoo.co.jp/qa/question_detail/q13298819348
1252982144さん

2024/6/6 23:02

0回答

visual studio2022において、TCP型においてクライアントで入力した文字列に従い、サーバで計算をするプログラムの問題で、画像の実行結果になるプログラムが分かりません。
もし分かる方いらっしゃいましたら、よろしくお願いします。

画像
C言語関連・47閲覧
《サーバ画面の出力例》
サーバ起動時のポート番号 (1024以上):10050
ポート番号 10050 を listning 中 (接続待ち)...
サーバ=127.0.0.1, ポート番号=3913 に接続.
add,10,20
10 + 20=30
sub,40,20
40 - 20 = 20
mul,8,5
8 * 5 = 40
[abc 100 200] の計算はできません
[exit] 数式ではありません


Enter キーを押してプログラム終了
《クライアント画面の出力例》
サーバのホスト名 (またはIPアドレス): localhost
サーバのポート番号 10050
実
サーバ [localhost] へ接続中… localhost に接続完了
文字列の入力: add,10,20
受信した文字列 : 10 + 20 = 30
文字列の入力: sub,40,20
受信した文字列 : 40-  20 = 20
文字列の入力:mul.8.5
文字列の入力: add,10,20
受信した文字列 : 8 * 5 = 40
文字列の入力: abc,100,200
受信した文字列 : [abc 100 200] の計算はできません
文字列の入力: exit
受信した文字列 : [exit] 数式ではありません

文字列の入力:.


Enter キーを押してプログラム終了
*/
