// Server.h : Copyright (C) 2024 https://www.youtube.com/@ProgrammerCpp
#include <Library/TCP.h>
#include <iostream>
struct binary_expression {
	char  ch ;
	int (*fn )(int lhs, int rhs);
	int   lhs;
	int   rhs;
	std::string evaluate() const
	{
		return (std::ostringstream() << lhs << ' ' << ch << ' ' << rhs << " = " << (*fn)(lhs, rhs)).str();
	}
};
void interact_with_client(const Library::TCP& s)
{
	for (std::stringstream stm;;) {
		const auto command = s.recv_message(stm);
		if (command == ".")
			return;
		else {
			std::istringstream istm(command);
			std::string op;
			int  lhs  ;
			int  rhs  ;
			char delim;
			if ((std::getline(istm, op, ',') >> lhs >> delim >> rhs) && delim == ',') {
				/**/ if (op == "add") std::cout << command << '\n' << s.send_message(binary_expression{'+', [](int lhs, int rhs)->int {return lhs + rhs; }, lhs, rhs}.evaluate()) << '\n';
				else if (op == "sub") std::cout << command << '\n' << s.send_message(binary_expression{'-', [](int lhs, int rhs)->int {return lhs - rhs; }, lhs, rhs}.evaluate()) << '\n';
				else if (op == "mul") std::cout << command << '\n' << s.send_message(binary_expression{'*', [](int lhs, int rhs)->int {return lhs * rhs; }, lhs, rhs}.evaluate()) << '\n';
				else
					std::cout << s.send_message('[' + command + "] の計算はできません") << '\n';
			}
			else
				std::cout << s.send_message('[' + command + "] 数式ではありません") << '\n';
			continue;
		}
	}
}
Library::TCP accept_single_connection(_In_ u_short const port_number)
{
	if (const Library::TCP s1{ socket(AF_INET, SOCK_STREAM, 0) }) {
		std::cout << "ポート番号 " << port_number << " を listening中(接続待ち)...";
		sockaddr_in addr;
		auto s2 = s1.bind(
			{ /*ADDRESS_FAMILY sin_family ;*/AF_INET
			, /*USHORT         sin_port   ;*/htons(port_number)
			, /*IN_ADDR        sin_addr   ;*/INADDR_ANY
			, /*CHAR           sin_zero[8];*/{}
			}).listen(SOMAXCONN).accept(addr);
		std::cout << '\n' << "サーバ=" << inet_ntoa(addr.sin_addr) << ", ポート番号=" << ntohs(addr.sin_port) << "に接続\n\n";
		return s2;
	}
	else
		throw std::runtime_error(__FILE__ "(" _CRT_STRINGIZE(__LINE__) ": socket failed with " + std::to_string(WSAGetLastError()));
}
#undef	max
int main(int argc, char** argv)
{
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 0), &d) == 0) {
		try {
			std::cout << "サーバ起動時のポート番号(1024以上): ";
			u_short port_number;
			if ((std::cin >> port_number).ignore(std::numeric_limits<std::streamsize>::max(), '\n')) {
				interact_with_client(accept_single_connection(port_number));
				std::cout << "Enter キーを押してプログラム終了\n";
				std::cin.get();
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
