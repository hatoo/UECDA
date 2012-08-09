
// indent -kr -ts4 
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "daihinmin.h"
#include "connection.h"
#include "mydef.h"
#include "cardChange.h"
#include "bitCard.h"
#include "cardSelect.h"
using namespace std;
const int g_logging = 0;		// ログ取りをするか否かを判定するための変数

int main(int argc, char *argv[])
{

	int my_playernum;			// プレイヤー番号を記憶する
	int whole_gameend_flag = 0;	// 全ゲームが終了したか否かを判別する変数
	int one_gameend_flag = 0;	// 1ゲームが終わったか否かを判別する変数
	int accept_flag = 0;		// 提出したカードが受理されたかを判別する変数
	int game_count = 0;			// ゲームの回数を記憶する

	int own_cards[8][15];		// 操作用の手札のテーブル
	int ba_cards[8][15];		// 操作用の場の札のテーブル

	changeInfo cinfo;
	fieldInfo finfo;
	// 引数のチェック
	// 引数に従ってサーバアドレス、接続ポート、クライアント名を変更
	checkArg(argc, argv);

	// ゲームに参加
	my_playernum = entryToGame();

	while (whole_gameend_flag == 0) {

		bool game_begin=true;

		one_gameend_flag = 0;	// 1ゲームが終わった事を示すフラグを初期化

		game_count = startGame(own_cards);	// ラウンドを始める
		// 最初のカードを受け取る。
		// /カード交換
		if (own_cards[5][0] == 0) {	// カード交換時フラグをチェック
			// ==1で正常
			printf("not card-change turn?\n");
			exit(1);
		} else {				// テーブルに問題が無ければ実際に交換へ
			if (own_cards[5][1] > 0 && own_cards[5][1] < 100) {
				int change_qty = own_cards[5][1];	// カードの交換枚数
				int select_cards[8][15] = { {0} };	// 選んだカードを格納

				// 自分が富豪、大富豪であれば不要なカードを選び出す

				// ///////////////////////////////////////////////////////////
				// カード交換のアルゴリズムはここに書く
				// ///////////////////////////////////////////////////////////

				//change(select_cards, own_cards, change_qty);
				checkCards(select_cards, own_cards, change_qty);

				// ///////////////////////////////////////////////////////////
				// カード交換のアルゴリズム ここまで
				// ///////////////////////////////////////////////////////////

				// 選んだカードを送信
				sendChangingCards(select_cards);
			} else {
				// 自分が平民以下なら、何かする必要はない

			}
		}						// カード交換ここまで

		while (one_gameend_flag == 0) {	// 1ゲームが終わるまでの繰り返し
			if(game_begin){
				game_begin=false;
				cinfo.firstPlayer=-1;
				finfo.goal=0;
			}

			int select_cards[8][15] = { {0} };	// 提出用のテーブル
			bool is_my_turn = receiveCards(own_cards);
			finfo.set(my_playernum, own_cards);

			if (is_my_turn) {	// カードをown_cardsに受け取り
				// 場を状態の読み出し
				// 自分のターンであるかを確認する
				// 自分のターンであればこのブロックが実行される。
				clearCards(select_cards);	// 選んだカードのクリア
				Cards myCards=setBit(own_cards);
				//finfo.onset=state.onset;
				selectHand(select_cards,finfo,myCards);
				/*
				// ///////////////////////////////////////////////////////////
				// アルゴリズムここから
				// どのカードを出すかはここにかく
				// ///////////////////////////////////////////////////////////
				if (state.onset == 1) {	// 場にカードが無いとき
					if (state.rev == 0) {
						lead(select_cards, own_cards);	// 通常時の提出用
					} else {
						leadRev(select_cards, own_cards);	// 革命時の提出用
					}
				} else {		// すでに場にカードがあるとき
					if (state.rev == 0) {
						follow(select_cards, own_cards);	// 通常時の提出用 
						// 
					} else {
						followRev(select_cards, own_cards);	// 革命時の提出用
					}
				}*/
				// ///////////////////////////////////////////////////////////
				// アルゴリズムはここまで
				// ///////////////////////////////////////////////////////////
				accept_flag = sendCards(select_cards);	// cardsを提出
				//cout << "frag= " << accept_flag << endl;
				if(accept_flag==8){
					bool f=false;
					for(int i=0;i<5;i++){
						for(int k=0;k<15;k++){
							if(select_cards[i][k]!=0)f=true;
						}
					}
					if(f){
					showState(&state);
					cerr << "error!!!" << endl;
					cerr << "onset= "<< (int)finfo.onset << endl;
					cerr << "qty= " << (int)finfo.qty << endl;
					cerr << "seq= " << (int)finfo.seq << endl;
					cerr << "ord= " << (int)finfo.ord << endl;
					cerr << "suit=" << (int)finfo.suit<< endl;
					cerr << "rev= " << (int)finfo.rev << endl;
					cerr << "lock=" << (int)finfo.rev << endl;
					cerr << endl;

					cerr << "tehuda" << endl;
					for(int i=0;i<5;i++){
						for(int k=0;k<15;k++){
							cerr << own_cards[i][k] << " ";
						}
						cerr << endl;
					}
					cerr << "selected" << endl;
					for(int i=0;i<5;i++){
						for(int k=0;k<15;k++){
							cerr << select_cards[i][k] << " ";
						}
						cerr << endl;
					}
					}
				}
			} else {
				// 自分のターンではない時
				// 必要ならここに処理を記述する
			}



			// そのターンに提出された結果のテーブル受け取り,場に出たカードの情報を解析する
			lookField(ba_cards);
			//最初に行動したプレイヤ
			if(cinfo.firstPlayer == -1){
				cinfo.firstPlayer = finfo.seat[ba_cards[5][3]];
			}
			finfo.set_ba(ba_cards);

			// /////////////////////////////////////////////////////////////
			// カードが出されたあと 誰かがカードを出す前の処理はここに書く
			// /////////////////////////////////////////////////////////////


			// /////////////////////////////////////////////////////////////
			// ここまで
			// /////////////////////////////////////////////////////////////

			// 一回のゲームが終わったか否かの通知をサーバからうける。
			switch (beGameEnd()) {
			case 0:			// 0のときゲームを続ける
				one_gameend_flag = 0;
				whole_gameend_flag = 0;
				break;
			case 1:			// 1のとき 1ゲームの終了
				one_gameend_flag = 1;
				whole_gameend_flag = 0;
				if (g_logging == 1) {
					printf("game #%d was finished.\n", game_count);
				}
				break;
			default:			// その他の場合 全ゲームの終了
				one_gameend_flag = 1;
				whole_gameend_flag = 1;
				if (g_logging == 1) {
					printf("All game was finished(Total %d games.)\n",
						   game_count);
				}
				break;
			}
		}						// 1ゲームが終わるまでの繰り返しここまで
	}							// 全ゲームが終わるまでの繰り返しここまで
	// ソケットを閉じて終了
	if (closeSocket() != 0) {
		printf("failed to close socket\n");
		exit(1);
	}
	exit(0);
}
