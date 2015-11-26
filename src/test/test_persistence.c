#include <stdio.h>
#include "persistence.h"
#include "soapH.h"

void test_list_friends(persistence *persistence){
	int i=0;
	psdims__user_list *friends=malloc(sizeof(psdims__user_list));

	get_list_friends(persistence,2,friends);

	while(i<friends->__sizenelems){
		printf("%s ,  %s\n",friends->user[i].name,friends->user[i].information);
		i++;
	}
}

void test_list_chats(persistence *persistence){
	int i=0,j;
	psdims__chat_list *chats=malloc(sizeof(psdims__chat_list));

	get_list_chats(persistence,2,chats);

	while(i<chats->__sizenelems){
		printf("Number chat=> %d, Admin chat=> %s , Description=> %s \n",chats->chat_info[i].chat_id,chats->chat_info[i].admin,chats->chat_info[i].description);
		for(j=0;j<chats->chat_info[i].members->__sizenelems;j++){
			printf("Miembro=> %s \n", chats->chat_info[i].members->name[j].string);
			}
		i++;
	}
}

void test_get_messages_request(persistence *persistence,int id_user,int id_chat){
	int i=0;
	psdims__message_list *messages=malloc(sizeof(psdims__message_list));

	if(exist_user_in_chat(persistence,id_user,id_chat)!=1){
		printf("No exite el usuario en ese chat\n");
		exit(1);
	}
		

	get_list_messages(persistence,id_chat,21,messages);

	while(i<messages->__sizenelems){
		printf("%s ,  %s ,  %d\n",messages->messages[i].user,messages->messages[i].text,messages->messages[i].send_date);
		i++;
	}
}

void test_send_request(persistence *persistence,int id_user,int id_request_name){
	int i=0;
	psdims__message_list *messages=malloc(sizeof(psdims__message_list));

	if(exist_friendly(persistence,id_user,id_request_name)!=0){
		printf("Ya son amigos\n");
		exit(1);
	}

	if(exist_request(persistence,id_user,id_request_name)!=0){
		printf("Ya existe una petición de amistad\n");
		exit(1);
	}

	send_request(persistence,id_user, id_request_name);
}

void test_accept_request(persistence *persistence,int id_user,int id_request_name){
	int i=0;
	psdims__message_list *messages=malloc(sizeof(psdims__message_list));

	if(exist_request(persistence,id_user,id_request_name)==0){
		printf("No existe una petición de amistad\n");
		exit(1);
	}
    
    accept_friend_request(persistence,id_user,id_request_name);
}

void test_decline_request(persistence *persistence,int id_user,int id_request_name){
	int i=0;
	psdims__message_list *messages=malloc(sizeof(psdims__message_list));

	if(exist_request(persistence,id_user,id_request_name)==0){
		printf("No existe una petición de amistad\n");
		exit(1);
	}
    
    decline_friend_request(persistence,id_user,id_request_name);
}

int main(int argc, char **argv){
	persistence *persistence=init_persistence(argv[1],argv[2]);	

	//test_get_messages_request(persistence,3,3);
	
	//test_list_chats(persistence);

	//test_list_friends(persistence);

	//test_send_request(persistence,1,4);

	//test_accept_request(persistence,1,4);

	test_decline_request(persistence,4,3);

	free_persistence(persistence);

	return 0;
}
