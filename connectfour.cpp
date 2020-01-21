#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Consumer.hpp"
#include "Producer.hpp"

#include "Semaphore.hpp"
#include <iostream>
#include <condition_variable>
#include <mutex>

#define TRUE 1
#define FALSE 0

void print(char (*m)[7], int moves, int player){
//print the game table

	int i;

	printf("\n   CONNECT FOUR - THE GAME\n\n");
	printf("     1  2  3  4  5  6  7 \n");


	for(i=0;i<6;i++){
			printf("    |%c||%c||%c||%c||%c||%c||%c| \n",
				m[i][0], m[i][1], m[i][2], m[i][3],
					m[i][4], m[i][5], m[i][6]);
	}

	printf("\n");
	printf("  Turn: Player %d\n", player);
	printf("  Moves %d\n", moves);
	printf("\n");
	return;
}


int check_win (char (*m)[7], int a, int b){
	int flag = 0;
	int i, j;
	int counter = 0;
	char c = m[a][b]; //indicates the symbol of the current player
	//vertical points
	for(i=a+1;m[i][b]== c && i<6; i++) counter++;
	for(i=a-1;m[i][b]== c && i>=0; i--) counter++;
	if(counter>=3) flag=1;
	counter = 0;

	//horizontal points
	for(i=b+1;m[a][i]==c && i<7; i++) counter++;
	for(i=b-1;m[a][i]==c && i>=0; i--) counter++;
	if(counter>=3) flag = 1;
	counter = 0;


	//diagonal 1
	for(i=a+1, j=b+1; m[i][j]==c && i<6 && j<7; i++, j++) counter++;
	for(i=a-1, j=b-1; m[i][j]==c && i>=0 && j>=0; i--, j--) counter++;
	if(counter>=3) flag = 1;
	counter = 0;

	//diagonal 2
	for(i=a-1, j=b+1; m[i][j]==c && i>=0 && j<7; i--, j++) counter++;
	for(i=a+1, j=b-1; m[i][j]==c && i<6 && j>=0; i++, j--) counter++;
	if(counter>=3) flag = 1;
	counter = 0;

	return flag;
}


int *valid_move(char (*m)[7]){

    int *v = (int *) malloc(sizeof(int)*7);
    //checks if the collum is full, if it is the v[i] = 0, invalid move
    for(int i=0; i<7; i++){
        if(m[0][i] == ' '){
            v[i] = 1;
        }else{
            v[i] = 0;
        }
    }

    return v;
}

int read_move(char (*m)[7], int player){
//recives the matrix, reads the move and checks if the game has ended. If the game is over read_move returns 1 otherwise it returns 0
	int a;
	int b;

	printf("  Number: ");
	scanf(" %d", &b);
	b--;


	for(a=0; m[a][b]==' ' && a<=5; a++) {
	};
	a--;

	if(player == 1){
		m[a][b] = 'X';
	}else{
		m[a][b] = 'O';
	}

	return check_win(m, a, b);
}

int set_move (char (*m)[7], int b){

    int a;

    for(a=0; m[a][b]==' ' && a<=5; a++) {
	};
	a--;

    m[a][b] = 'O';

	return check_win(m, a, b);
}


void call_solver(int *past_moves, int *valid, int moves, Producer *p, Consumer *c){

	std::string s = "";
	for (int i = 0; i < moves; ++i) {
		s += std::to_string(past_moves[i]+1);
	}
	
	for(int i=0; i<7; i++){
		if(valid[i] == 1){
			Work w = Work(s+std::to_string(i+1));
			(*p).add_work(w);			
		}
	}
	(*c).get_all();
	
	std::cout << "call_solver ";
	std::cout << s << std::endl;
	

	return;
}


int main(){

	int i, j;
	int a, b;
	int past_moves[50];
	char m[6][7];
	int flag = 0;
	int moves = 0;
	int player = 1;
	int *valid;
	int game_mode;
	int *score;

	//fills all the matrix with blanked spaces
	for(i=0;i<6;i++)
		for(j=0;j<7;j++)
			m[i][j] = ' ';

	printf("Welcome to Connetc Four - The Game \n\n Select the number of players: (1 or 2)\n\n Number of players: ");
	scanf("%d", &game_mode);





if(game_mode == 2){ //PLAYER 2 MODE

            do{
                print(m, moves, player);//print

                flag = read_move(m, player); //reads the move and check if the game is over

                if(player==1){ //alternates the player
                    player = 2;
                }else{
                    player = 1;
                }

                moves++; //increment the number of moves
                printf("\n\n");

                system("clear");

            }while(flag!=TRUE);//flag is 1 when the game is over

            if(player==1){//returns to the winner
                    player = 2;
            }else{
                    player = 1;
            }

            printf("\n\nPLAYER %d WINS!\n", player);
            printf("Score: %d\n", moves);

}else{ //PLAYER 1 VS PC


			    // Inicializa as filas de entrada e saida de dados
    std::deque<Work> w_q;
    std::deque<Result> r_q;
    // Referencias (para compartilhar)
    std::deque<Work>* ref_w = &w_q;
    std::deque<Result>* ref_r = &r_q;

    // Controle da região critica para o semaforos
    std::condition_variable cv_w;
    std::condition_variable cv_r;
    // Referências
    std::condition_variable* ref_cvw = &cv_w;
    std::condition_variable* ref_cvr = &cv_r;

    // Mutex para os semaforos
    std::mutex mtx_w;
    std::mutex mtx_r;
    // Referencias
    std::mutex* ref_mtxw = &mtx_w;
    std::mutex* ref_mtxr = &mtx_r;

    // Constroi os semaforos
    Semaphore w_sem = Semaphore(ref_cvw, ref_mtxw, 0);
    Semaphore r_sem = Semaphore(ref_cvr, ref_mtxr, 0);
    // Referencias (para compartilhar)
    Semaphore* ref_wsem = &w_sem;
    Semaphore* ref_rsem = &r_sem;

    // Instancia o Producer
    Producer p = Producer(ref_w, ref_wsem);
    Consumer c = Consumer(ref_w, ref_wsem, ref_r, ref_rsem);


            do{
                //PLAYER 1 MOVES
                print(m, moves, player);//print

                printf("  Number: ");
                scanf(" %d", &b);
                b--;

                for(a=0; m[a][b]==' ' && a<=5; a++) { //piece falls to the bottom of the table
                };
                a--;
                m[a][b]  = 'X';
                past_moves[moves] = b;
                moves++;
                
                valid = valid_move(m);

				//score = 
                call_solver(past_moves, valid, moves, &p, &c);

				//PUT IT BACK SOMEDAY//moves++; //increment the number of moves

                system("clear");

            }while(flag!=TRUE);//flag is 1 when the game is over

}





return 0;

}
