#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "stack.h"

typedef struct {
	pthread_cond_t queue_cv ;
	pthread_cond_t dequeue_cv ;
	pthread_mutex_t lock ;
	char ** elem ;
	int capacity ;
	int num ; 
	int front ;
	int rear ;
} bounded_buffer ;

// BOARD_SIZE를 정의하여 N값 변경 가능
#ifndef BOARD_SIZE
#define BOARD_SIZE 4	
#endif 

bounded_buffer * buf = 0x0 ;
int thread_count = 0;
int total_count = 0;
pthread_mutex_t print_mutex;

void bounded_buffer_init (bounded_buffer * buf, int capacity) {
	pthread_cond_init(&(buf->queue_cv), 0x0) ;
	pthread_cond_init(&(buf->dequeue_cv), 0x0) ;
	pthread_mutex_init(&(buf->lock), 0x0) ;
	buf->capacity = capacity ;
	buf->elem = (char **) calloc(sizeof(char *), capacity) ;
	buf->num = 0 ;
	buf->front = 0 ;
	buf->rear = 0 ;
}

void bounded_buffer_queue (bounded_buffer * buf, void * msg) {
	pthread_mutex_lock(&(buf->lock)) ;
		
	while (!(buf->num < buf->capacity)) {
		pthread_cond_wait(&(buf->queue_cv), &(buf->lock)) ;
	}

	buf->elem[buf->rear] = msg ;
	buf->rear = (buf->rear + 1) % buf->capacity ;
	buf->num += 1 ;	

	pthread_cond_signal(&(buf->dequeue_cv)) ;
	pthread_mutex_unlock(&(buf->lock)) ;
}

void * bounded_buffer_dequeue (bounded_buffer * buf) {
	void * r = NULL;

	pthread_mutex_lock(&(buf->lock)) ;

	while (!(0 < buf->num)) {
		pthread_cond_wait(&(buf->dequeue_cv), &(buf->lock)) ;
	}

	r = buf->elem[buf->front] ;
	buf->front = (buf->front + 1) % buf->capacity ;
	buf->num -= 1 ;

	pthread_cond_signal(&(buf->queue_cv)) ;
	pthread_mutex_unlock(&(buf->lock)) ;

	return r ;
}




// 동시 실행할 스레드 개수 관련 명령행
void validation_opt(int argc, char *argv[])
{
    int option;

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

void signal_handler(int sig) {
	if(sig == SIGINT) {
		printf("Total Count : %d\n", total_count);
		exit(0);
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

void print_placement (struct stack_t * queens) {
	
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
	return total_count;
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
	return total_count;
}


void * 
producer (void * ptr) 
{
	struct stack_t *queens = create_stack(((struct stack_t *)ptr)->capacity);
	push(queens, 0);
	bounded_buffer_queue(buf, (void *)queens);

	while (!is_empty(queens)) {
		int latest_queen;
		top(queens, &latest_queen);

		if (latest_queen == BOARD_SIZE * BOARD_SIZE) {
			pop(queens, &latest_queen);
			if (!is_empty(queens)) {
				pop(queens, &latest_queen);
				push(queens, latest_queen + 1);
				bounded_buffer_queue(buf, create_stack(queens->capacity));
			} else {
				break;
			}
			continue;
		}

		if (is_feasible(queens)) {
			if (get_size(queens) == BOARD_SIZE) {
				bounded_buffer_queue(buf, create_stack(queens->capacity));
				int latest_queen;
				pop(queens, &latest_queen);
				push(queens, latest_queen + 1);
			} else {
				push(queens, latest_queen + 1);
				bounded_buffer_queue(buf, create_stack(queens->capacity));
			}
		} else {
			int latest_queen;
			pop(queens, &latest_queen);
			push(queens, latest_queen + 1);
		}
	}
	delete_stack(queens);
	return NULL;
}

void * 
consumer (void * ptr) 
{
	while (1) {
		struct stack_t *queens = bounded_buffer_dequeue(buf);
		if (is_feasible(queens) && get_size(queens) == BOARD_SIZE) {
			pthread_mutex_lock(&print_mutex);
			print_placement(queens);
			printf("\n");
			total_count++;
			pthread_mutex_unlock(&print_mutex);
		}
		delete_stack(queens);
	}
	return NULL;
}



int main (int argc, char *argv[]) {
	validation_opt(argc, argv); // 스레드 개수 관련 명령행 함수 호출

	pthread_t prod[thread_count] ;
	pthread_t cons[thread_count] ;

	int i ;

	buf = malloc(sizeof(bounded_buffer)) ;
	bounded_buffer_init(buf, 3) ;

	signal(SIGINT, signal_handler);


	for (i = 0 ; i <  thread_count; i++) {
		pthread_create(&(prod[i]), 0x0 , producer, 0x0) ;
		pthread_create(&(cons[i]), 0x0, consumer, 0x0) ;
	}

	for (i = 0 ; i < thread_count ; i++) {
		pthread_join(prod[i], 0x0) ;
		pthread_join(cons[i], 0x0) ;
	}

	free(buf);
	
	return EXIT_FAILURE ;
}
