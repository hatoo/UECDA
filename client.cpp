
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
const int g_logging = 0;		// ���O�������邩�ۂ��𔻒肷�邽�߂̕ϐ�

int main(int argc, char *argv[])
{

	int my_playernum;			// �v���C���[�ԍ����L������
	int whole_gameend_flag = 0;	// �S�Q�[�����I���������ۂ��𔻕ʂ���ϐ�
	int one_gameend_flag = 0;	// 1�Q�[�����I��������ۂ��𔻕ʂ���ϐ�
	int accept_flag = 0;		// ��o�����J�[�h���󗝂��ꂽ���𔻕ʂ���ϐ�
	int game_count = 0;			// �Q�[���̉񐔂��L������

	int own_cards[8][15];		// ����p�̎�D�̃e�[�u��
	int ba_cards[8][15];		// ����p�̏�̎D�̃e�[�u��

	changeInfo cinfo;
	fieldInfo finfo;
	// �����̃`�F�b�N
	// �����ɏ]���ăT�[�o�A�h���X�A�ڑ��|�[�g�A�N���C�A���g����ύX
	checkArg(argc, argv);

	// �Q�[���ɎQ��
	my_playernum = entryToGame();

	while (whole_gameend_flag == 0) {

		bool game_begin=true;

		one_gameend_flag = 0;	// 1�Q�[�����I��������������t���O��������

		game_count = startGame(own_cards);	// ���E���h���n�߂�
		// �ŏ��̃J�[�h���󂯎��B
		// /�J�[�h����
		if (own_cards[5][0] == 0) {	// �J�[�h�������t���O���`�F�b�N
			// ==1�Ő���
			printf("not card-change turn?\n");
			exit(1);
		} else {				// �e�[�u���ɖ�肪������Ύ��ۂɌ�����
			if (own_cards[5][1] > 0 && own_cards[5][1] < 100) {
				int change_qty = own_cards[5][1];	// �J�[�h�̌�������
				int select_cards[8][15] = { {0} };	// �I�񂾃J�[�h���i�[

				// �������x���A��x���ł���Εs�v�ȃJ�[�h��I�яo��

				// ///////////////////////////////////////////////////////////
				// �J�[�h�����̃A���S���Y���͂����ɏ���
				// ///////////////////////////////////////////////////////////

				//change(select_cards, own_cards, change_qty);
				checkCards(select_cards, own_cards, change_qty);

				// ///////////////////////////////////////////////////////////
				// �J�[�h�����̃A���S���Y�� �����܂�
				// ///////////////////////////////////////////////////////////

				// �I�񂾃J�[�h�𑗐M
				sendChangingCards(select_cards);
			} else {
				// �����������ȉ��Ȃ�A��������K�v�͂Ȃ�

			}
		}						// �J�[�h���������܂�

		while (one_gameend_flag == 0) {	// 1�Q�[�����I���܂ł̌J��Ԃ�
			if(game_begin){
				game_begin=false;
				cinfo.firstPlayer=-1;
				finfo.goal=0;
			}

			int select_cards[8][15] = { {0} };	// ��o�p�̃e�[�u��
			bool is_my_turn = receiveCards(own_cards);
			finfo.set(my_playernum, own_cards);

			if (is_my_turn) {	// �J�[�h��own_cards�Ɏ󂯎��
				// �����Ԃ̓ǂݏo��
				// �����̃^�[���ł��邩���m�F����
				// �����̃^�[���ł���΂��̃u���b�N�����s�����B
				clearCards(select_cards);	// �I�񂾃J�[�h�̃N���A
				Cards myCards=setBit(own_cards);
				//finfo.onset=state.onset;
				selectHand(select_cards,finfo,myCards);
				/*
				// ///////////////////////////////////////////////////////////
				// �A���S���Y����������
				// �ǂ̃J�[�h���o�����͂����ɂ���
				// ///////////////////////////////////////////////////////////
				if (state.onset == 1) {	// ��ɃJ�[�h�������Ƃ�
					if (state.rev == 0) {
						lead(select_cards, own_cards);	// �ʏ펞�̒�o�p
					} else {
						leadRev(select_cards, own_cards);	// �v�����̒�o�p
					}
				} else {		// ���łɏ�ɃJ�[�h������Ƃ�
					if (state.rev == 0) {
						follow(select_cards, own_cards);	// �ʏ펞�̒�o�p 
						// 
					} else {
						followRev(select_cards, own_cards);	// �v�����̒�o�p
					}
				}*/
				// ///////////////////////////////////////////////////////////
				// �A���S���Y���͂����܂�
				// ///////////////////////////////////////////////////////////
				accept_flag = sendCards(select_cards);	// cards���o
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
				// �����̃^�[���ł͂Ȃ���
				// �K�v�Ȃ炱���ɏ������L�q����
			}



			// ���̃^�[���ɒ�o���ꂽ���ʂ̃e�[�u���󂯎��,��ɏo���J�[�h�̏�����͂���
			lookField(ba_cards);
			//�ŏ��ɍs�������v���C��
			if(cinfo.firstPlayer == -1){
				cinfo.firstPlayer = finfo.seat[ba_cards[5][3]];
			}
			finfo.set_ba(ba_cards);

			// /////////////////////////////////////////////////////////////
			// �J�[�h���o���ꂽ���� �N�����J�[�h���o���O�̏����͂����ɏ���
			// /////////////////////////////////////////////////////////////


			// /////////////////////////////////////////////////////////////
			// �����܂�
			// /////////////////////////////////////////////////////////////

			// ���̃Q�[�����I��������ۂ��̒ʒm���T�[�o���炤����B
			switch (beGameEnd()) {
			case 0:			// 0�̂Ƃ��Q�[���𑱂���
				one_gameend_flag = 0;
				whole_gameend_flag = 0;
				break;
			case 1:			// 1�̂Ƃ� 1�Q�[���̏I��
				one_gameend_flag = 1;
				whole_gameend_flag = 0;
				if (g_logging == 1) {
					printf("game #%d was finished.\n", game_count);
				}
				break;
			default:			// ���̑��̏ꍇ �S�Q�[���̏I��
				one_gameend_flag = 1;
				whole_gameend_flag = 1;
				if (g_logging == 1) {
					printf("All game was finished(Total %d games.)\n",
						   game_count);
				}
				break;
			}
		}						// 1�Q�[�����I���܂ł̌J��Ԃ������܂�
	}							// �S�Q�[�����I���܂ł̌J��Ԃ������܂�
	// �\�P�b�g����ďI��
	if (closeSocket() != 0) {
		printf("failed to close socket\n");
		exit(1);
	}
	exit(0);
}
