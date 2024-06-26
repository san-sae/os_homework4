#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "stack.h"

// BOARD_SIZE를 정의하여 N값 변경 가능
#ifndef BOARD_SIZE
#define BOARD_SIZE 4	
#endif 

// 동시 실행할 스레드 개수 관련 명령행
void validation_opt(int argc, char *argv[])
{
    int option;
	int thread_count = 0;

    while ((option = getopt(argc, argv, "c:")) != -1){
        switch (option)
        {
        case 'c':
            thread_count = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Usage: ./%s -c <thread_count> \n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}




// 결과는 위치번호 cell을 통해 행(P/N)과 열(P%N)로 나타냄
int row (int cell){
	return cell / BOARD_SIZE ;
}

int col (int cell){
	return cell % BOARD_SIZE ;
}


int is_feasible (struct stack_t * queens) 
{
	int board[BOARD_SIZE][BOARD_SIZE] ;
	int c, r ;

	for (r = 0 ; r < BOARD_SIZE ; r++) {
		for (c = 0 ; c < BOARD_SIZE ; c++) {
			board[r][c] = 0 ;
		}
	}

	for (int i = 0 ; i < get_size(queens) ; i++) {
		int cell ;
		get_elem(queens, i, &cell) ;
		
		int r = row(cell) ;
		int c = col(cell) ;
	
		if (board[r][c] != 0) {
			return 0 ;
		}

		int x, y ;
		for (y = 0 ; y < BOARD_SIZE ; y++) {
			board[y][c] = 1 ;
		}
		for (x = 0 ; x < BOARD_SIZE ; x++) {
			board[r][x] = 1 ;
		}

		y = r + 1 ; x = c + 1 ;
		while (0 <= y && y < BOARD_SIZE && 0 <= x && x < BOARD_SIZE) {
			board[y][x] = 1 ;
			y += 1 ; x += 1 ;
		}

		y = r + 1 ; x = c - 1 ;
		while (0 <= y && y < BOARD_SIZE && 0 <= x && x < BOARD_SIZE) {
			board[y][x] = 1 ;
			y += 1 ; x -= 1 ;
		}

		y = r - 1 ; x = c + 1 ;
		while (0 <= y && y < BOARD_SIZE && 0 <= x && x < BOARD_SIZE) {
			board[y][x] = 1 ;
			y -= 1 ; x += 1 ;
		}

		y = r - 1 ; x = c - 1 ;
		while (0 <= y && y < BOARD_SIZE && 0 <= x && x < BOARD_SIZE) {
			board[y][x] = 1 ;
			y -= 1 ; x -= 1 ;
		}

	}

	return 1;
}

void print_placement (struct stack_t * queens)
{
	for (int i = 0 ; i < queens->size ; i++) {	
		int queen ;
		get_elem(queens, i, &queen) ;
		printf("[%d,%d] ", row(queen), col(queen)) ;
	}
}


int find_n_queens_with_prepositions (int N, struct stack_t * prep)
{
	struct stack_t * queens = create_stack(BOARD_SIZE) ;

	queens->capacity = prep->capacity ;
	queens->size = prep->size ;
	memcpy(queens->buffer, prep->buffer, prep->size * sizeof(int)) ;

	while (prep->size <= queens->size) {
		int latest_queen ;
		top(queens, &latest_queen) ;

		if (latest_queen == BOARD_SIZE * BOARD_SIZE) {
			pop(queens, &latest_queen) ;
			if (!is_empty(queens)) {
				pop(queens, &latest_queen) ;
				push(queens, latest_queen + 1) ;
			}
			else {
				break ;
			}
			continue ;
		}

		if (is_feasible(queens)) {
			if (get_size(queens) == N) {
			
				print_placement(queens) ;

				printf("\n") ;

				int lastest_queen ;
				pop(queens, &latest_queen) ;
				push(queens, latest_queen + 1) ;
			}
			else {
				int latest_queen ;
				top(queens, &latest_queen) ;
				push(queens, latest_queen + 1) ;
			}
		}
		else {
			int lastest_queen ;
			pop(queens, &latest_queen) ;
			push(queens, latest_queen + 1) ;
		}

	}
	delete_stack(queens) ;
}

int find_n_queens (int N)
{
	struct stack_t * queens = create_stack(BOARD_SIZE) ;

	push(queens, 0) ;
	while (!is_empty(queens)) {
		int latest_queen ;
		top(queens, &latest_queen) ;

		if (latest_queen == BOARD_SIZE * BOARD_SIZE) {
			pop(queens, &latest_queen) ;
			if (!is_empty(queens)) {
				pop(queens, &latest_queen) ;
				push(queens, latest_queen + 1) ;
			}
			else {
				break ;
			}
			continue ;
		}

		if (is_feasible(queens)) {
			if (get_size(queens) == N) {

				print_placement(queens) ;
				printf("\n") ;

				int lastest_queen ;
				pop(queens, &latest_queen) ;
				push(queens, latest_queen + 1) ;
			}
			else {
				int latest_queen ;
				top(queens, &latest_queen) ;
				push(queens, latest_queen + 1) ;
			}
		}
		else {
			int lastest_queen ;
			pop(queens, &latest_queen) ;
			push(queens, latest_queen + 1) ;
		}

	}
	delete_stack(queens) ;
}

int main (int argc, char *argv[]) {
	validation_opt(argc, argv); // 스레드 개수 관련 명령행 함수 호출

	find_n_queens(4) ;
	return EXIT_FAILURE ;
}
