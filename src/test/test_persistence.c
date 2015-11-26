#include <stdio.h>
#include "persistence.h"
#include "soapH.h"

void test_list_friends(persistence *persistence){
	int i=0;
	psdims__user_list *friends=malloc(sizeof(psdims__user_list));

	get_list_friends(persistence,2,friends);

	while(i<friends->__sizenelems){
		printf("%s ,  %s\n",friends->user_info[i].name,friends->user_info[i].information);
		i++;
	}
}

void test_list_chats(persistence *persistence){
	int i=0;
	psdims__chat_list *chats=malloc(sizeof(psdims__chat_list));

	get_list_chats(persistence,1,chats);

	while(i<chats->__sizenelems){
		printf("Number chat=> %d, Admin chat=> %s , Description=> %s\n",chats->chat_info[i].chat_id,chats->chat_info[i].admin,chats->chat_info[i].description);
		i++;
	}
}

int main(int argc, char **argv){
	persistence *persistence=init_persistence(argv[1],argv[2]);	

	test_list_chats(persistence);
	
	free_persistence(persistence);

	return 0;
}
